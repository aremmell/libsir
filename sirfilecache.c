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

    if (!_sir_sanity())
        return NULL;

    sirfcache* sfc = _sir_locksection(SIRMI_FILECACHE);
    if (!sfc) {
        _sir_seterror(_SIR_E_INTERNAL);
        return NULL;
    }

    _sir_defaultlevels(&levels, sir_file_def_lvls);
    _sir_defaultopts(&opts, sir_file_def_opts);

    sirfileid_t retval = _sir_fcache_add(sfc, path, levels, opts);
    _sir_unlocksection(SIRMI_FILECACHE);

    return retval;
}

bool _sir_updatefile(sirfileid_t id, sir_update_config_data* data) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validptr(id) || !_sir_validfd(*id) ||
        !_sir_validupdatedata(data))
            return false;

    sirfcache* sfc = _sir_locksection(SIRMI_FILECACHE);
    if (!sfc) {
        _sir_seterror(_SIR_E_INTERNAL);
        return false;
    }

    bool retval = _sir_fcache_update(sfc, id, data);
    _sir_unlocksection(SIRMI_FILECACHE);

    return retval;
}

bool _sir_remfile(sirfileid_t id) {
    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validptr(id) || !_sir_validfd(*id))
        return false;

    sirfcache* sfc = _sir_locksection(SIRMI_FILECACHE);
    if (!sfc) {
        _sir_seterror(_SIR_E_INTERNAL);
        return false;
    }

    bool retval = _sir_fcache_rem(sfc, id);
    _sir_unlocksection(SIRMI_FILECACHE);

    return retval;
}

sirfile* _sirfile_create(const char* path, sir_levels levels, sir_options opts) {
    if (!_sir_validstr(path) || !_sir_validlevels(levels) || !_sir_validopts(opts))
        return NULL;

    sirfile* sf = (sirfile*)calloc(1, sizeof(sirfile));
    if (!sf) {
        _sir_handleerr(errno);
        return NULL;
    }

    size_t pathLen = strnlen(path, SIR_MAXPATH);
    sf->path       = (char*)calloc(pathLen + 1, sizeof(char));
    if (!sf->path) {
        _sir_handleerr(errno);
        _sir_safefree(sf);
        return NULL;
    }

    _sir_strncpy(sf->path, pathLen + 1, path, pathLen);

    sf->levels = levels;
    sf->opts   = opts;

    if (!_sirfile_open(sf) || !_sirfile_validate(sf)) {
        _sir_safefree(sf->path);
        _sir_safefree(sf);
        return NULL;
    }

    return sf;
}

bool _sirfile_open(sirfile* sf) {
    if (!_sir_validptr(sf) && !_sir_validstr(sf->path))
        return false;

    FILE* f  = NULL;
    int open = _sir_fopen(&f, sf->path, SIR_FOPENMODE);
    if (0 != open || !f)
        return false;

    int fd = fileno(f);
    if (!_sir_validfd(fd))
        return false;

    _sirfile_close(sf);

    sf->f  = f;
    sf->id = fd;

    return true;
}

void _sirfile_close(sirfile* sf) {
    if (!_sir_validptrnofail(sf) || !_sir_validptrnofail(sf->f))
        return;

    _sir_fclose(&sf->f);
}

bool _sirfile_write(sirfile* sf, const char* output) {
    if (!_sirfile_validate(sf) || !_sir_validstr(output))
        return false;

    if (_sirfile_needsroll(sf)) {
        bool rolled   = false;
        char* newpath = NULL;

        _sir_selflog("file %d (path: '%s') reached ~%ld bytes in size; rolling...",
            sf->id, sf->path, SIR_FROLLSIZE);

        _sir_fflush(sf->f);

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
        _sir_handleerr(errno);
        clearerr(sf->f);
#pragma message("TODO: keep an error counter on each file. if errors continue to pile up, remove from cache")
    }

    return write == writeLen;
}

bool _sirfile_writeheader(sirfile* sf, const char* msg) {
    if (!_sirfile_validate(sf) || !_sir_validstr(msg))
        return false;

    time_t now = -1;
    time(&now);

    char timestamp[SIR_MAXTIME] = {0};
    bool fmttime = _sir_formattime(now, timestamp, SIR_FHTIMEFORMAT);
    if (!fmttime)
        return false;

    char header[SIR_MAXFHEADER] = {0};
    int fmt = snprintf(header, SIR_MAXFHEADER, SIR_FHFORMAT, msg, timestamp);

    if (0 > fmt) {
        _sir_handleerr(errno);
        return false;
    }

    return 0 <= fmt && _sirfile_write(sf, header);
}

bool _sirfile_needsroll(sirfile* sf) {
    if (!_sirfile_validate(sf))
        return false;

    struct stat st = {0};
    int getstat    = fstat(sf->id, &st);

    if (0 != getstat) {
        _sir_handleerr(errno);
        return false;
    }

    return (st.st_size + BUFSIZ) >= SIR_FROLLSIZE;
}

bool _sirfile_roll(sirfile* sf, char** newpath) {
    if (!_sirfile_validate(sf) || !_sir_notnull(newpath))
        return false;

    bool retval = false;
    char* name = NULL;
    char* ext  = NULL;

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
                        int print = snprintf(*newpath, SIR_MAXPATH, SIR_FNAMEFORMAT, name,
                            timestamp, (sequence > 0 ? seqbuf : ""),
                            _sir_validstrnofail(ext) ? ext : "");

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
                            /* the file already exists; add a number to the file name
                                * until one that does not exist is found. */
                            _sir_selflog("path: '%s' already exists; incrementing sequence",
                                *newpath);
                            sequence++;
                        } else {
                            _sir_selflog("found good path: '%s'", *newpath);
                            resolved = true;
                            break;
                        }

                        if (sequence > 0) {
                            print = snprintf(seqbuf, 7, SIR_FNAMESEQFORMAT, sequence);

                            if (print < 0) {
                                _sir_handleerr(errno);
                                break;
                            }
                        }

                    } while (sequence <= 999);

                    if (!resolved)
                        _sir_selflog("error: unable to determine suitable path for '%s';"
                                        " not rolling!", sf->path);

                    retval = resolved && _sirfile_archive(sf, *newpath);
                }
            }
        }
    }

    _sir_safefree(name);
    _sir_safefree(ext);

    return retval;

}

bool _sirfile_archive(sirfile* sf, const char* newpath) {
    if (!_sirfile_validate(sf) || !_sir_validstr(newpath))
        return false;

#if defined(__WIN__)
    /* apparently, need to close the old file first on windows. */
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

    return false;
}

bool _sirfile_splitpath(sirfile* sf, char** name, char** ext) {
    if (NULL != name)
        *name = NULL;
    if (NULL != ext)
        *ext = NULL;

    if (!_sirfile_validate(sf) || !_sir_validptr(name) || !_sir_validptr(ext))
        return false;

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

void _sirfile_destroy(sirfile* sf) {
    if (!sf)
        return;

    _sirfile_close(sf);
    _sir_safefree(sf->path);
    _sir_safefree(sf);
}

bool _sirfile_validate(sirfile* sf) {
    return _sir_validptrnofail(sf) && _sir_validptrnofail(sf->f) &&
           _sir_validstrnofail(sf->path) && _sir_validfd(sf->id);
}

bool _sirfile_update(sirfile* sf, sir_update_config_data* data) {
    if (!_sirfile_validate(sf))
        return false;

    if (_sir_bittest(data->fields, SIRU_LEVELS)) {
        if (sf->levels != *data->levels) {
            _sir_selflog("updating file %d levels from %04" PRIx16 " to %04" PRIx16,
                sf->id, sf->levels, *data->levels);
            sf->levels = *data->levels;
        } else {
            _sir_selflog("skipped superfluous update of file %d levels: %04" PRIx16,
                sf->id, sf->levels);
        }

        return true;
    }

    if (_sir_bittest(data->fields, SIRU_OPTIONS)) {
        if (sf->opts != *data->opts) {
            _sir_selflog("updating file %d options from %08" PRIx32 " to %08" PRIx32, sf->id,
                sf->opts, *data->opts);
            sf->opts = *data->opts;
        } else {
            _sir_selflog("skipped superfluous update of file %d options: %08" PRIx32, sf->id,
                sf->opts);
        }

        return true;
    }

    return false;
}

sirfileid_t _sir_fcache_add(sirfcache* sfc, const char* path, sir_levels levels,
    sir_options opts) {
    if (!_sir_validptr(sfc) || !_sir_validstr(path) || !_sir_validlevels(levels) ||
        !_sir_validopts(opts))
        return NULL;

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

    return NULL;
}

bool _sir_fcache_update(sirfcache* sfc, sirfileid_t id, sir_update_config_data* data) {
    if (!_sir_validptr(sfc) || !_sir_validptr(id) || !_sir_validfd(*id) ||
        !_sir_validupdatedata(data))
        return false;

    sirfile* found = _sir_fcache_find(sfc, (const void*)id, _sir_fcache_pred_id);
    if (!found) {
        _sir_seterror(_SIR_E_NOFILE);
        return false;
    }

    return _sirfile_update(found, data);
}

bool _sir_fcache_rem(sirfcache* sfc, sirfileid_t id) {
    if (!_sir_validptr(sfc) || !_sir_validptr(id) || !_sir_validfd(*id))
        return false;

    for (size_t n = 0; n < sfc->count; n++) {
        assert(_sirfile_validate(sfc->files[n]));

        if (sfc->files[n]->id == *id) {
            _sirfile_destroy(sfc->files[n]);

            for (size_t i = n; i < sfc->count - 1; i++) {
                sfc->files[i] = sfc->files[i + 1];
                sfc->files[i + 1] = NULL;
            }

            sfc->count--;
            return true;
        }
    }

    _sir_seterror(_SIR_E_NOFILE);
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
    if (!_sir_validptr(sfc) || !_sir_validptr(match) || !_sir_notnull(pred))
        return NULL;

    for (size_t n = 0; n < sfc->count; n++) {
        if (pred(match, sfc->files[n]))
            return sfc->files[n];
    }

    return NULL;
}

bool _sir_fcache_destroy(sirfcache* sfc) {
    if (!_sir_validptr(sfc))
        return false;

    for (size_t n = 0; n < sfc->count; n++) {
        assert(_sirfile_validate(sfc->files[n]));
        _sirfile_destroy(sfc->files[n]);
        sfc->files[n] = NULL;
        sfc->count--;
    }

    memset(sfc, 0, sizeof(sirfcache));
    return true;
}

bool _sir_fcache_dispatch(sirfcache* sfc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted) {
    if (!_sir_validptr(sfc) || !_sir_validlevel(level) || !_sir_validptr(buf) ||
        !_sir_validptr(dispatched) || !_sir_validptr(wanted))
        return false;

    bool retval = true;
    const char* write = NULL;
    sir_options lastopts = 0;

    *dispatched = 0;
    *wanted = 0;

    for (size_t n = 0; n < sfc->count; n++) {
        assert(_sirfile_validate(sfc->files[n]));

        if (!_sir_bittest(sfc->files[n]->levels, level)) {
            _sir_selflog("level %04" PRIx16 " not set in level mask (%04" PRIx16
                         ") for file %d (path: '%s'); skipping",
                level, sfc->files[n]->levels, sfc->files[n]->id, sfc->files[n]->path);
            continue;
        }

        (*wanted)++;

        if (!write || sfc->files[n]->opts != lastopts) {
            write = _sir_format(false, sfc->files[n]->opts, buf);
            assert(write);
            lastopts = sfc->files[n]->opts;
        }

        if (write && _sirfile_write(sfc->files[n], write)) {
            retval &= true;
            (*dispatched)++;
        } else {
            _sir_selflog("error: write to file %d (path: '%s') failed!", sfc->files[n]->id,
                sfc->files[n]->path);
        }
    }

    return retval && (*dispatched == *wanted);
}

void _sir_fclose(FILE** f) {
    if (!_sir_validptr(f) || !_sir_validptr(*f))
        return;

    if (0 != fclose(*f))
        _sir_handleerr(errno);
    *f = NULL;
}

void _sir_fflush(FILE* f) {
    if (!_sir_validptr(f))
        return;

    if (0 != fflush(f)) {
        _sir_handleerr(errno);
        return;
    }
}
