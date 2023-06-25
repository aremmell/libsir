/*
 * sirfilecache.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
 * License:   The MIT License (MIT)
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
#include "sirfilesystem.h"
#include "sirinternal.h"
#include "sirdefaults.h"
#include "sirmutex.h"

sirfileid_t _sir_addfile(const char* path, sir_levels levels, sir_options opts) {

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

bool _sir_updatefile(sirfileid_t id, sir_update_config_data* data) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validptr(id) && _sir_validfid(*id) &&
        _sir_validupdatedata(data)) {
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);
        assert(sfc);

        if (sfc) {
            bool updated = _sir_fcache_update(sfc, id, data);
            if (!updated)
                _sir_selflog("error: file update failed!");
            return _sir_unlocksection(_SIRM_FILECACHE) && updated;
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

sirfile* _sirfile_create(const char* path, sir_levels levels, sir_options opts) {

    sirfile* sf = NULL;

    if (_sir_validstr(path) && _sir_validlevels(levels) && _sir_validopts(opts)) {

        sf = (sirfile*)calloc(1, sizeof(sirfile));

        if (_sir_validptr(sf)) {
            size_t pathLen = strnlen(path, SIR_MAXPATH);
            sf->path       = (char*)calloc(pathLen + 1, sizeof(char));

            if (_sir_validptrnofail(sf->path)) {
                _sir_strncpy(sf->path, pathLen + 1, path, pathLen);
                
                sf->levels = levels;
                sf->opts   = opts;

                if (!_sirfile_open(sf) || !_sirfile_validate(sf))
                    _sir_safefree(sf);
            }
        }
    }

    return sf;
}

bool _sirfile_open(sirfile* sf) {

    if (_sir_validptr(sf) && _sir_validstr(sf->path)) {
        
        FILE* f  = NULL;
        int open = _sir_fopen(&f, sf->path, SIR_FOPENMODE);
        if (0 == open && f) {
            int fd = fileno(f);
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
        if (_sir_validptrnofail(sf->f) && _sir_validfid(sf->id)) {
            _sir_fflush(sf->f);
            _sir_fclose(&sf->f);
            sf->id = -1;
        }
    }
}

bool _sirfile_write(sirfile* sf, const char* output) {

    if (_sirfile_validate(sf) && _sir_validstr(output)) {
        
        if (_sirfile_needsroll(sf)) {
            bool rolled = false;
            char* newpath = NULL;
            
            if (_sirfile_roll(sf, &newpath)) {
                char header[SIR_MAXFHEADER] = {0};
                snprintf(header, SIR_MAXFHEADER, SIR_FHROLLED, newpath);
                rolled = _sirfile_writeheader(sf, header);
            }

            _sir_safefree(newpath);
            if (!rolled) /* write anyway; don't want to lose data. */
                _sir_selflog("error: failed to roll file %d (path: '%s')!",
                    sf->id, sf->path);
        }

        size_t writeLen = strnlen(output, SIR_MAXFHEADER);
        size_t write    = fwrite(output, sizeof(char), writeLen, sf->f);

        assert(write == writeLen);

        if (write < writeLen) {
            int err = ferror(sf->f);
            int eof = feof(sf->f);

            _sir_selflog("error: incomplete write of %zu/%zu bytes to file %d (path: '%s')!"
                         " ferror: %d, feof: %d, path: '%s'", write, writeLen,
                         sf->id, sf->path, err, eof, sf->path);

            /**
             * If an error occurs on write, consider removing file from targets,
             * or at least attempt to roll the file (out of space?)
             */
#pragma message("TODO: Handle write failure according to error code")            

            clearerr(sf->f);
        }

        return write == writeLen;
    }

    return false;
}

bool _sirfile_writeheader(sirfile* sf, const char* msg) {

    if (_sirfile_validate(sf) && _sir_validstr(msg)) {
        time_t now = -1;
        time(&now);

        char timestamp[SIR_MAXTIME] = {0};
        bool fmttime = _sir_formattime(now, timestamp, SIR_FHTIMEFORMAT);
        assert(fmttime);

        if (fmttime) {
            char header[SIR_MAXFHEADER] = {0};
            int fmt = snprintf(header, SIR_MAXFHEADER, SIR_FHFORMAT, msg, timestamp);

            if (fmt < 0)
                _sir_handleerr(errno);

            return fmt >=0 && _sirfile_write(sf, header);
        }
    }

    return false;
}

bool _sirfile_needsroll(sirfile* sf) {
    
    if (_sirfile_validate(sf)) {
        struct stat st = {0};
        int getstat    = fstat(sf->id, &st);

        if (0 != getstat) {
            _sir_handleerr(errno);
            return false;
        }

        return st.st_size >= SIR_FROLLSIZE;
    }

    return false;
}

bool _sirfile_roll(sirfile* sf, char** newpath) {

    if (_sirfile_validate(sf) && newpath) {

        bool r = false;
        char* name = NULL;
        char* ext = NULL;

        bool split = _sirfile_splitpath(sf, &name, &ext);
        assert(split);

        if (split) {
            time_t now = -1;
            
            time(&now);
            assert(-1 != now);

            if (-1 != now) {
                char timestamp[SIR_MAXTIME] = {0};
                bool fmttime = _sir_formattime(now, timestamp, SIR_FNAMETIMEFORMAT);
                assert(fmttime);
                
                if (fmttime) {
                    *newpath = (char*)calloc(SIR_MAXPATH, sizeof(char));

                    if (_sir_validptr(*newpath)) {
                        char seqbuf[7] = {0};
                        bool exists = false;
                        bool resolved = false;
                        uint16_t sequence = 0;

                        do {
                            int print = snprintf(*newpath, SIR_MAXPATH, SIR_FNAMEFORMAT "%s",
                                name, timestamp, _sir_validstrnofail(ext) ? ext : "",
                                sequence > 0 ? seqbuf : "");

                            if (print < 0) {
                                _sir_handleerr(errno);
                                break;
                            }

                            /* if less than one second has elasped since the last roll
                            * operation, then we'll overwrite the last rolled log file,
                            * and that = data loss. make sure the target path does not
                            * already exist. */
                            if (!_sir_pathexists(*newpath, &exists, SIR_PATH_REL_TO_CWD)) {
                                /* failed to determine if the file already exists; it is better
                                 * to continue logging to the same file than to possibly overwrite
                                 * another (if it failed this time, it will again, so there's no
                                 * way to definitively choose a good new path). */
                                break;
                            } else if (exists) {
                                /* the file already exists; concatentate a number to the end of
                                 * the file name until one that does not exist is found. */
                                _sir_selflog("path: '%s' already exists; incrementing sequence", *newpath);
                                sequence++;
                            } else {
                                _sir_selflog("found good path: '%s'", *newpath);
                                resolved = true;
                                break;
                            }

                            if (sequence > 0) {
                                print = snprintf(seqbuf, 7, "-%hu", sequence);
                                
                                if (print < 0) {
                                    _sir_handleerr(errno);
                                    break;
                                }
                            }

                        } while (sequence <= 999);

                        if (!resolved)
                            _sir_selflog("error: unable to determine suitable roll path for '%s';"
                                         " not rolling!", sf->path);

                        r = resolved && _sirfile_archive(sf, *newpath);
                    }
                }
            }
        }

        _sir_safefree(name);
        _sir_safefree(ext);

        return r;
    }

    return false;
}

bool _sirfile_archive(sirfile* sf, const char* newpath) {

    if (_sirfile_validate(sf) && _sir_validstr(newpath)) {
#if defined(__WIN__)
        /* apparently need to close the old file first on windows. */
        _sirfile_close(sf);
#endif
        if (0 != rename(sf->path, newpath)) {
            _sir_handleerr(errno);
            return false;
        }

        if (_sirfile_open(sf)) {
            _sir_selflog("archived '%s' " SIR_R_ARROW " '%s'", sf->path, newpath);
            return true;
        }
    }

    return false;
}

bool _sirfile_splitpath(sirfile* sf, char** name, char** ext) {

    if (NULL != name) *name = NULL;
    if (NULL != ext) *ext = NULL;

    if (_sirfile_validate(sf) && _sir_validptr(name) && _sir_validptr(ext)) {

        char* lastfullstop = strrchr(sf->path, '.');

        if (lastfullstop) {
            uintptr_t namesize = lastfullstop - sf->path;
            assert(namesize < SIR_MAXPATH);

            if (namesize < SIR_MAXPATH) {
                *name = (char*)calloc(namesize + 1, sizeof(char));
                _sir_strncpy(*name, namesize + 1, sf->path, namesize);
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

bool _sirfile_update(sirfile* sf, sir_update_config_data* data) {
    if (!_sirfile_validate(sf))
        return false;

    if (_sir_bittest(data->fields, SIRU_LEVELS)) {
        if (sf->levels != *data->levels) {
            _sir_selflog("updating file %d levels from %04" PRIx16 " to %04" PRIx16, sf->id,
                sf->levels, *data->levels);
            sf->levels = *data->levels;
        } else {
            _sir_selflog("skipped superfluous update of file %d levels: %04" PRIx16, sf->id, sf->levels);
        }

        return true;
    }

    if (_sir_bittest(data->fields, SIRU_OPTIONS)) {
        if (sf->opts != *data->opts) {
            _sir_selflog("updating file %d options from %08" PRIx32 " to %08" PRIx32, sf->id,
                sf->opts, *data->opts);
            sf->opts = *data->opts;
        } else {
            _sir_selflog("skipped superfluous update of file %d options: %08" PRIx32, sf->id, sf->opts);
        }

        return true;
    }

    return false;
}

sirfileid_t _sir_fcache_add(sirfcache* sfc, const char* path, sir_levels levels, sir_options opts) {

    if (_sir_validptr(sfc) && _sir_validstr(path) && _sir_validlevels(levels) && _sir_validopts(opts)) {

        if (sfc->count >= SIR_MAXFILES) {
            _sir_seterror(_SIR_E_FCFULL);
            return NULL;
        }

        sirfile* existing = _sir_fcache_find(sfc, (const void*)path, _sir_fcache_pred_path);

        if (NULL != existing) {
            _sir_seterror(_SIR_E_DUPFILE);
            _sir_selflog("error: already managing file with path '%s'", path);
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

bool _sir_fcache_update(sirfcache* sfc, sirfileid_t id, sir_update_config_data* data) {

    if (_sir_validptr(sfc) && _sir_validptr(id) &&
        _sir_validfid(*id) && _sir_validupdatedata(data)) {
        sirfile* found = _sir_fcache_find(sfc, (const void*)id, _sir_fcache_pred_id);
        if (!found) {
            _sir_seterror(_SIR_E_NOFILE);
            return false;
        }

        return _sirfile_update(found, data);
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
    const char* path = (const char*)match;
#if !defined(__WIN__)
    return 0 == strncmp(path, iter->path, SIR_MAXPATH);
#else /* __WIN__ */
    /* paths/file names are not case sensitive on windows. */
    return 0 == _strnicmp(path, iter->path, SIR_MAXPATH);
#endif
}

bool _sir_fcache_pred_id(const void* match, sirfile* iter) {
    sirfileid_t id = (sirfileid_t)match;
    return iter->id == *id;
}

sirfile* _sir_fcache_find(sirfcache* sfc, const void* match, sir_fcache_pred pred) {

    if (_sir_validptr(sfc) && _sir_validptr(match) && _sir_notnull(pred)) {
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

bool _sir_fcache_dispatch(sirfcache* sfc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted) {

    if (_sir_validptr(sfc) && _sir_validlevel(level) && _sir_validptr(buf)
        && _sir_validptr(dispatched) && _sir_validptr(wanted)) {

        bool r = true;
        const char* write = NULL;
        sir_options lastopts = 0;

        *dispatched = 0;
        *wanted = 0;

        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (!_sir_bittest(sfc->files[n]->levels, level)) {
                _sir_selflog("level %04 " PRIx16 " not set in level mask (%04" PRIx16 ") for file %d; skipping",
                    level, sfc->files[n]->levels, sfc->files[n]->id);
                continue;
            }

            (*wanted)++;

            if (!write || sfc->files[n]->opts != lastopts) {
                write = _sir_format(false, sfc->files[n]->opts, buf);
                assert(write);
                lastopts = sfc->files[n]->opts;
            }

            if (write && _sirfile_write(sfc->files[n], write)) {
                r &= true;
                (*dispatched)++;
            } else {
                _sir_selflog("write to file %d failed! path: '%s'", sfc->files[n]->id, sfc->files[n]->path);
            }
        }

        if (*dispatched > 0)
            _sir_fflush_all();

        return r && (*dispatched == *wanted);
    }

    return false;
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
