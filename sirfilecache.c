/**
 * @file sirfilecache.c
 * @brief Internal log file management.
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "sirfilecache.h"
#include "sirinternal.h"
#include "sirdefaults.h"
#include "sirmutex.h"

/**
 * @addtogroup intern
 * @{
 */

sirfileid_t _sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity()) {
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);
        assert(sfc);

        if (sfc) {
            _sir_defaultlevels(&levels, sir_file_def_lvls);
            _sir_defaultopts(&opts, sir_file_def_opts);            

            sirfileid_t r = _sir_fcache_add(sfc, path, levels, opts);
            _sir_unlocksection(_SIRM_FILECACHE);
            return r;
        }
    }

    return NULL;
}

bool _sir_updatefile(sirfileid_t id, sir_update_data* data) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validptr(id) && _sir_validfid(*id) &&
        _sir_validupdatedata(data)) {
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);
        assert(sfc);

        if (sfc) {
            bool r = _sir_fcache_update(sfc, id, data);
            return _sir_unlocksection(_SIRM_FILECACHE) && r;
        }
    }

    return false;
}

bool _sir_remfile(sirfileid_t id) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validptr(id) && _sir_validfid(*id)) {
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);
        assert(sfc);

        if (sfc) {
            bool r = _sir_fcache_rem(sfc, id);
            return _sir_unlocksection(_SIRM_FILECACHE) && r;
        }
    }

    return false;
}

sirfile* _sirfile_create(const sirchar_t* path, sir_levels levels, sir_options opts) {

    sirfile* sf = NULL;

    if (_sir_validstr(path) && _sir_validlevels(levels) && _sir_validopts(opts)) {

        sf = (sirfile*)calloc(1, sizeof(sirfile));

        if (_sir_validptr(sf)) {
            size_t pathLen = strnlen(path, SIR_MAXPATH);
            sf->path       = (sirchar_t*)calloc(pathLen + 1, sizeof(sirchar_t));

            if (_sir_validptr(sf->path)) {
                strncpy(sf->path, path, pathLen);
                
                sf->levels = levels;
                sf->opts = opts;

                if (!_sirfile_open(sf) || !_sirfile_validate(sf))
                    _sir_safefree(sf);
            }
        }
    }

    return sf;
}

bool _sirfile_open(sirfile* sf) {

    if (_sir_validptr(sf) && _sir_validstr(sf->path)) {

        FILE* f = _sir_fopen(sf->path);

        if (f) {            
            int fd = fileno(f);
            if (-1 == fd)
                _sir_handleerr(errno);

            if (_sir_validfid(fd)) {
                _sirfile_close(sf);
                                
                sf->f  = f;
                sf->id = fd;     
                return true;
            }
        }
    }

    return false;
}

void _sirfile_close(sirfile* sf) {
    if (_sir_validptr(sf)) {
        if (_sir_validptr(sf->f) && _sir_validfid(sf->id)) {
            _sir_fflush(sf->f);
            _sir_fclose(&sf->f);
            sf->id = SIR_INVALID;
        }
    }
}

bool _sirfile_write(sirfile* sf, const sirchar_t* output) {

    if (_sirfile_validate(sf) && _sir_validstr(output)) {

        if (_sirfile_needsroll(sf)) {
            bool rolled = false;
            sirchar_t* newpath = NULL;
            
            if (_sirfile_roll(sf, &newpath)) {
                sirchar_t header[SIR_MAXMESSAGE] = {0};
                snprintf(header, SIR_MAXMESSAGE, SIR_FHROLLED, newpath);
                rolled = _sirfile_writeheader(sf, header);
            }

            _sir_safefree(newpath);
            if (!rolled) return false;
        }

        size_t writeLen = strnlen(output, SIR_MAXOUTPUT);
        size_t write    = fwrite(output, sizeof(sirchar_t), writeLen, sf->f);

        assert(write == writeLen);

        if (write < writeLen) {
            int err = ferror(sf->f);
            int eof = feof(sf->f);

            _sir_handleerr(err);
            _sir_handleerr(eof);

            _sir_selflog("%s: wrote %lu/%lu bytes to %d; ferror: %d, feof: %d\n", __func__, write, writeLen,
                sf->id, err, eof);

            /** @todo
             * If an error occurs on write, consider removing file from targets,
             * or at least attempt to roll the file (out of space?)
             */

            clearerr(sf->f);
        }

        return write == writeLen;
    }

    return false;
}

bool _sirfile_writeheader(sirfile* sf, const sirchar_t* msg) {

    if (_sirfile_validate(sf) && _sir_validstr(msg)) {
        time_t now;
        bool   gettime = _sir_getlocaltime(&now, NULL);
        assert(gettime);

        if (gettime) {
            sirchar_t timestamp[SIR_MAXTIME] = {0};
            bool      fmttime           = _sir_formattime(now, timestamp, SIR_FHTIMEFORMAT);
            assert(fmttime);

            if (fmttime) {
                sirchar_t header[SIR_MAXOUTPUT] = {0};
                int fmt = snprintf(header, SIR_MAXOUTPUT, SIR_FHFORMAT, msg, timestamp);

                if (fmt < 0)
                    _sir_handleerr(errno);

                return fmt >=0 && _sirfile_write(sf, header);
            }
        }
    }

    return false;
}

bool _sirfile_needsroll(sirfile* sf) {

    if (_sirfile_validate(sf)) {
        struct stat st      = {0};
        int         getstat = fstat(sf->id, &st);

        if (0 != getstat) {
            _sir_handleerr(errno);
            return false;
        }

        return st.st_size >= SIR_FROLLSIZE;
    }

    return false;
}

bool _sirfile_roll(sirfile* sf, sirchar_t** newpath) {

    if (_sirfile_validate(sf) && newpath) {

            bool r = false;
            sirchar_t* name = NULL;
            sirchar_t* ext = NULL;

            bool split = _sirfile_splitpath(sf, &name, &ext);
            assert(split);

            if (split) {
                time_t now;
                
                bool gettime = _sir_getlocaltime(&now, NULL);
                assert(gettime);

                if (gettime) {                
                    sirchar_t timestamp[SIR_MAXTIME] = {0};
                    bool fmttime = _sir_formattime(now, timestamp, SIR_FNAMETIMEFORMAT);
                    assert(fmttime);
                    
                    if (fmttime) {
                        *newpath = (sirchar_t*)calloc(SIR_MAXPATH, sizeof(sirchar_t));

                        if (_sir_validptr(*newpath)) {
                            int fmtpath = snprintf(*newpath, SIR_MAXPATH, SIR_FNAMEFORMAT,
                                name, timestamp, _sir_validstrnofail(ext) ? ext : "");                

                            if (fmtpath < 0)
                                _sir_handleerr(errno);

                            r = fmtpath >= 0 && _sirfile_archive(sf, *newpath);
                        }
                    }
                }

                _sir_safefree(name);
                _sir_safefree(ext);
            }

        return r;
    }

    return false;
}

/** @todo compress archived log files */
bool _sirfile_archive(sirfile* sf, const sirchar_t* newpath) {

    if (_sirfile_validate(sf) && _sir_validstr(newpath)) {
#ifdef _WIN32
        /* apparently need to close the old file first on windows. */
        _sirfile_close(sf);
#endif
        if (0 != rename(sf->path, newpath)) {
            _sir_handleerr(errno);
            return false;
        }

        if (_sirfile_open(sf)) {
            _sir_selflog("%s: archived '%s' -> '%s'\n", __func__, sf->path, newpath);
            return true;
        }
    }

    return false;
}

bool _sirfile_splitpath(sirfile* sf, sirchar_t** name, sirchar_t** ext) {

    if (name) *name = NULL;
    if (ext) *ext = NULL;

    if (_sirfile_validate(sf) && _sir_validptr(name) && _sir_validptr(ext)) {

        sirchar_t* lastfullstop = strrchr(sf->path, '.');

        if (lastfullstop) {
            uintptr_t namesize = lastfullstop - sf->path;
            assert(namesize < SIR_MAXPATH);

            if (namesize < SIR_MAXPATH) {
                *name = (sirchar_t*)calloc(namesize + 1, sizeof(sirchar_t));
                strncpy(*name, sf->path, namesize);
            }
  
            *ext = strdup(lastfullstop);
        } else {
            *name = strdup(sf->path);
        }

        return _sir_validstr(*name) && (!lastfullstop || _sir_validstr(*ext));
    }

    return false;
}

void _sirfile_destroy(sirfile* sf) {
    if (sf) {
        _sirfile_close(sf);
        _sir_safefree(sf->path);
        _sir_safefree(sf);
    }
}

bool _sirfile_validate(sirfile* sf) {
    return _sir_validptr(sf)    && _sir_validfid(sf->id) &&
           _sir_validptr(sf->f) && _sir_validstr(sf->path);
}

void  _sirfile_update(sirfile* sf, sir_update_data* data) {
    if (_sirfile_validate(sf) && _sir_validupdatedata(data)) {
        if (data->levels && _sir_validlevels(*data->levels))
            sf->levels = *data->levels;
        if (data->opts && _sir_validopts(*data->opts))
            sf->opts = *data->opts;
    }
}

sirfileid_t _sir_fcache_add(sirfcache* sfc, const sirchar_t* path, sir_levels levels, sir_options opts) {

    if (_sir_validptr(sfc) && _sir_validstr(path) && _sir_validlevels(levels) && _sir_validopts(opts)) {

        if (sfc->count >= SIR_MAXFILES) {
            _sir_seterror(_SIR_E_FCFULL);
            return NULL;
        }

        sirfile* existing = _sir_fcache_find(sfc, (const void*)path, _sir_fcache_pred_path);

        if (NULL != existing) {
            _sir_seterror(_SIR_E_DUPFILE);
            _sir_selflog("%s: file with path '%s' already added.\n", __func__, path);
            return NULL;
        }

        sirfile* sf = _sirfile_create(path, levels, opts);

        if (_sirfile_validate(sf)) {
            sfc->files[sfc->count++] = sf;

            if (!_sir_bittest(sf->opts, SIRO_NOHDR))
                _sirfile_writeheader(sf, SIR_FHBEGIN);

            return &sf->id;
        }
    }

    return NULL;
}

bool _sir_fcache_update(sirfcache* sfc, sirfileid_t id, sir_update_data* data) {

    if (_sir_validptr(sfc) && _sir_validptr(id) && _sir_validfid(*id) &&
        _sir_validupdatedata(data)) {
        sirfile* found = _sir_fcache_find(sfc, (const void*)id, _sir_fcache_pred_id);
        if (!found) {
            _sir_seterror(_SIR_E_NOFILE);
            return false;
        }

        _sirfile_update(found, data);
        return true;
    }

    return false;
}

bool _sir_fcache_rem(sirfcache* sfc, sirfileid_t id) {

    if (_sir_validptr(sfc) && _sir_validptr(id) && _sir_validfid(*id)) {
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (sfc->files[n]->id == *id) {
                _sirfile_destroy(sfc->files[n]);

                for (size_t i = n; i < sfc->count - 1; i++) {
                    sfc->files[i] = sfc->files[i + 1];
                    sfc->files[i + 1] = NULL;
                }

                assert(sfc->count > 0);
                sfc->count--;

                return true;
            }
        }

        _sir_seterror(_SIR_E_NOFILE);
    }

    return false;
}

bool _sir_fcache_pred_path(const void* match, sirfile* iter) {
    const sirchar_t* path = (const sirchar_t*)match;
#ifndef _WIN32
    return 0 == strncmp(path, iter->path, SIR_MAXPATH);
#else
    /* paths/file names are not case sensitive on windows. */
    return 0 == _strnicmp(path, iter->path, SIR_MAXPATH);
#endif
}

bool _sir_fcache_pred_id(const void* match, sirfile* iter) {
    sirfileid_t id = (sirfileid_t)match;
    return iter->id == *id;
}

sirfile* _sir_fcache_find(sirfcache* sfc, const void* match, sir_fcache_pred pred) {

    if (_sir_validptr(sfc) && _sir_validptr(match) && _sir_validptr(pred)) {
        for (size_t n = 0; n < sfc->count; n++) {
            if (pred(match, sfc->files[n]))
                return sfc->files[n];
        }
    }

    return NULL;
}

bool _sir_fcache_destroy(sirfcache* sfc) {

    if (_sir_validptr(sfc)) {
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));
            _sirfile_destroy(sfc->files[n]);
            sfc->files[n] = NULL;
            assert(sfc->count > 0);
            sfc->count--;
        }

        memset(sfc, 0, sizeof(sirfcache));
        return true;
    }

    return false;
}

bool _sir_fcache_dispatch(sirfcache* sfc, sir_level level, siroutput* output,
    size_t* dispatched, size_t* wanted) {

    if (_sir_validptr(sfc) && _sir_validlevel(level) && _sir_validptr(output)
        && _sir_validptr(dispatched) && _sir_validptr(wanted)) {

        bool r = true;
        const sirchar_t* write = NULL;
        sir_options lastopts = 0;

        *dispatched = 0;
        *wanted = 0;

        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (!_sir_bittest(sfc->files[n]->levels, level)) {
                _sir_selflog("%s: levels for %d (%04lx) not set for (%04lx); skipping...\n", __func__,
                    sfc->files[n]->id, sfc->files[n]->levels, level);
                continue;
            }

            (*wanted)++;

            if (!write || sfc->files[n]->opts != lastopts) {
                write = _sir_format(false, sfc->files[n]->opts, output);
                assert(write);
                lastopts = sfc->files[n]->opts;
            }

            if (write && _sirfile_write(sfc->files[n], write)) {
                r &= true;
                (*dispatched)++;
            } else {
                _sir_selflog("%s: write to %d failed!\n", __func__, sfc->files[n]->id);
            }
        }

        if (*dispatched > 0)
            _sir_fflush_all();

        return r && (*dispatched == *wanted);
    }

    return false;
}

FILE* _sir_fopen(const sirchar_t* path) {
    if (_sir_validstr(path)) {
#ifdef __STDC_SECURE_LIB__
        FILE*   tmp  = NULL;
        errno_t open = fopen_s(&tmp, path, SIR_FOPENMODE);
        _sir_handleerr(open);
        return tmp;
#else
        FILE *f = fopen(path, SIR_FOPENMODE);
        if (!f) _sir_handleerr(errno);
        return f;
#endif
    }

    return NULL;
}

void _sir_fclose(FILE** f) {
    if (_sir_validptr(f) && _sir_validptr(*f)) {
        if (0 != fclose(*f)) _sir_handleerr(errno);
        *f = NULL;
    }    
}

void _sir_fflush(FILE* f) {
    if (_sir_validptr(f)) {
        if (0 != fflush(f)) _sir_handleerr(errno);
    }
}

bool _sir_fflush_all(void) {
    if (0 != fflush(NULL)) {
        _sir_handleerr(errno);
        return false;
    }
    return true;
}

/** @} */
