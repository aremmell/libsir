/*
 * sirfilecache.c
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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
 *
 * -----------------------------------------------------------------------------
 */

//-V::522
#include "sir/filecache.h"
#include "sir/filesystem.h"
#include "sir/internal.h"
#include "sir/defaults.h"

sirfileid _sir_addfile(const char* path, sir_levels levels, sir_options opts) {
    (void)_sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity())
        return 0U;

    _SIR_LOCK_SECTION(sirfcache, sfc, SIRMI_FILECACHE, 0U);

    _sir_defaultlevels(&levels, sir_file_def_lvls);
    _sir_defaultopts(&opts, sir_file_def_opts);

    sirfileid retval = _sir_fcache_add(sfc, path, levels, opts);
    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);

    return retval;
}

bool _sir_updatefile(sirfileid id, const sir_update_config_data* data) {
    (void)_sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validfileid(id) || !_sir_validupdatedata(data))
        return false;

    _SIR_LOCK_SECTION(const sirfcache, sfc, SIRMI_FILECACHE, false);
    bool retval = _sir_fcache_update(sfc, id, data);
    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);

    return retval;
}

bool _sir_remfile(sirfileid id) {
    (void)_sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validfileid(id))
        return false;

    _SIR_LOCK_SECTION(sirfcache, sfc, SIRMI_FILECACHE, false);
    bool retval = _sir_fcache_rem(sfc, id);
    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);

    return retval;
}

sirfile* _sirfile_create(const char* path, sir_levels levels, sir_options opts) {
    if (!_sir_validstr(path) || !_sir_validlevels(levels) || !_sir_validopts(opts))
        return NULL;

    sirfile* sf = (sirfile*)calloc(1, sizeof(sirfile));
    if (!sf) {
        (void)_sir_handleerr(errno);
        return NULL;
    }

    sf->path = strndup(path, strnlen(path, SIR_MAXPATH));
    if (!sf->path) {
        (void)_sir_handleerr(errno);
        _sir_safefree(&sf);
        return NULL;
    }

    sf->levels = levels;
    sf->opts   = opts;

    if (!_sirfile_open(sf) || !_sirfile_validate(sf)) {
        _sirfile_destroy(&sf);
        return NULL;
    }

    return sf;
}

bool _sirfile_open(sirfile* sf) {
    bool retval = _sir_validptr(sf) && _sir_validstr(sf->path);

    if (retval) {
        FILE* f  = NULL;
        retval = _sir_openfile(&f, sf->path, SIR_FOPENMODE, SIR_PATH_REL_TO_CWD);
        if (retval && NULL != f) {
            _sirfile_close(sf);

            sf->f  = f;
            sf->id = FNV32_1a((const uint8_t*)sf->path, strnlen(sf->path, SIR_MAXPATH));
        }
    }

    return retval;
}

void _sirfile_close(sirfile* sf) {
    if (_sir_validptrnofail(sf))
        _sir_safefclose(&sf->f);
}

bool _sirfile_write(sirfile* sf, const char* output) {
    bool retval = _sirfile_validate(sf) && _sir_validstr(output);

    if (retval) {
        if (sf->writes_since_size_chk++ > SIR_FILE_CHK_SIZE_WRITES) {
            sf->writes_since_size_chk = 0;
            _sirfile_rollifneeded(sf);
        }

        size_t writeLen = strnlen(output, SIR_MAXOUTPUT);
        size_t wrote    = fwrite(output, sizeof(char), writeLen, sf->f);

        SIR_ASSERT(wrote == writeLen);

        if (wrote < writeLen) {
            (void)_sir_handleerr(errno);
            clearerr(sf->f);
        }

        retval = wrote == writeLen;
    }

    return retval;
}

bool _sirfile_writeheader(sirfile* sf, const char* msg) {
    bool retval = _sirfile_validate(sf) && _sir_validstr(msg);

    if (retval) {
        time_t now = -1;
        (void)time(&now);

        char timestamp[SIR_MAXTIME] = {0};
        bool fmttime = _sir_formattime(now, timestamp, SIR_FHTIMEFORMAT);
        if (!fmttime)
            return false;

        char header[SIR_MAXFHEADER] = {0};
        (void)snprintf(header, SIR_MAXFHEADER, SIR_FHFORMAT, msg, timestamp);

        retval = _sirfile_write(sf, header);
    }

    return retval;
}

bool _sirfile_needsroll(sirfile* sf) {
    bool retval = _sirfile_validate(sf);

    if (retval) {
        struct stat st = {0};
        int getstat    = fstat(fileno(sf->f), &st);

        if (0 != getstat) { /* if fstat fails, try stat on the path. */
            getstat = stat(sf->path, &st);
            if (0 != getstat)
                return _sir_handleerr(errno);
        }

        retval = st.st_size + BUFSIZ >= SIR_FROLLSIZE ||
            SIR_FROLLSIZE - (st.st_size + BUFSIZ) <= BUFSIZ;
    }

    return retval;
}

bool _sirfile_roll(sirfile* sf, char** newpath) {
    if (!_sirfile_validate(sf) || !_sir_validptrptr(newpath))
        return false;

    bool retval = false;
    char* name = NULL;
    char* ext  = NULL;

    bool split = _sirfile_splitpath(sf, &name, &ext);
    SIR_ASSERT(split);

    if (split) {
        time_t now = -1;

        (void)time(&now);
        SIR_ASSERT(-1 != now);

        if (-1 != now) {
            char timestamp[SIR_MAXTIME] = {0};
            bool fmttime = _sir_formattime(now, timestamp, SIR_FNAMETIMEFORMAT);
            SIR_ASSERT(fmttime);

            if (fmttime) {
                *newpath = (char*)calloc(SIR_MAXPATH, sizeof(char));

                if (_sir_validptr(*newpath)) {
                    char seqbuf[7] = {0};
                    bool exists = false;
                    bool resolved = false;
                    uint16_t sequence = 0U;

                    do {
                        (void)snprintf(*newpath, SIR_MAXPATH, SIR_FNAMEFORMAT, name,
                            timestamp, (sequence > 0U ? seqbuf : ""),
                            _sir_validstrnofail(ext) ? ext : "");

                        /* if less than one second has elapsed since the last roll
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
                            _sir_selflog("path: '%s' already exists; incrementing sequence", *newpath); //-V576
                            sequence++;
                        } else {
                            _sir_selflog("found good path: '%s'", *newpath);
                            resolved = true;
                            break;
                        }

                        if (sequence > 0)
                            (void)snprintf(seqbuf, 7, SIR_FNAMESEQFORMAT, sequence);

                    } while (sequence <= 999U);

                    if (!resolved)
                        _sir_selflog("error: unable to determine suitable path for '%s';"
                                        " not rolling!", sf->path);

                    retval = resolved && _sirfile_archive(sf, *newpath);
                }
            }
        }
    }

    _sir_safefree(&name);
    _sir_safefree(&ext);

    return retval;
}

void _sirfile_rollifneeded(sirfile* sf) {
    if (_sirfile_validate(sf) && _sirfile_needsroll(sf)) {
        bool rolled   = false;
        char* newpath = NULL;

        _sir_selflog("file (path: '%s', id: %"PRIx32") reached ~%d bytes in size;"
            " rolling...", sf->path, sf->id, SIR_FROLLSIZE);

        _sir_fflush(sf->f);

        if (_sirfile_roll(sf, &newpath)) {
            char header[SIR_MAXFHEADER] = {0};
            (void)snprintf(header, SIR_MAXFHEADER, SIR_FHROLLED, newpath);
            rolled = _sirfile_writeheader(sf, header);
        }

        _sir_safefree(&newpath);
        if (!rolled) /* write anyway; don't want to lose data. */
            _sir_selflog("error: failed to roll file (path: '%s', id: %"PRIx32")!",
                sf->path, sf->id);
    }
}

bool _sirfile_archive(sirfile* sf, const char* newpath) {
    bool retval = _sirfile_validate(sf) && _sir_validstr(newpath);

    if (retval) {
#if defined(__WIN__)
        /* apparently, need to close the old file first on windows. */
        _sirfile_close(sf);
#endif
        if (0 != rename(sf->path, newpath)) {
            retval = _sir_handleerr(errno);
        } else {
            retval = _sirfile_open(sf);
            if (retval)
                _sir_selflog("archived '%s' " SIR_R_ARROW " '%s'", sf->path, newpath);
        }
    }

    return retval;
}

bool _sirfile_splitpath(const sirfile* sf, char** name, char** ext) {
    if (_sir_validptrptr(name))
        *name = NULL;
    if (_sir_validptrptr(ext))
        *ext = NULL;

    bool retval = _sirfile_validate(sf) && _sir_validptrptr(name) && _sir_validptrptr(ext);

    if (retval) {
        char* tmp = strndup(sf->path, strnlen(sf->path, SIR_MAXPATH));
        if (!tmp)
            return _sir_handleerr(errno);

        const char* fullstop = strrchr(tmp, '.');
        if (fullstop && fullstop != tmp) {
            uintptr_t namesize = fullstop - tmp;
            SIR_ASSERT(namesize < SIR_MAXPATH);

            tmp[namesize] = '\0';
            *name = (char*)calloc(namesize + 1, sizeof(char));
            if (!*name) {
                _sir_safefree(&tmp);
                return _sir_handleerr(errno);
            }

            (void)_sir_strncpy(*name, namesize + 1, tmp, namesize);
            *ext = strndup(sf->path + namesize, strnlen(sf->path + namesize, SIR_MAXPATH));
        } else {
            fullstop = NULL;
            *name = strndup(sf->path, strnlen(sf->path, SIR_MAXPATH));
        }

        _sir_safefree(&tmp);
        retval = _sir_validstrnofail(*name) && (!fullstop || _sir_validstrnofail(*ext));
    }

    return retval;
}

void _sirfile_destroy(sirfile** sf) {
    if (sf && *sf) {
        _sirfile_close(*sf);
        _sir_safefree(&(*sf)->path);
        _sir_safefree(sf);
    }
}

bool _sirfile_validate(const sirfile* sf) {
    return _sir_validptrnofail(sf) && _sir_validptrnofail(sf->f) &&
           _sir_validstrnofail(sf->path) && _sir_validfileid(sf->id);
}

bool _sirfile_update(sirfile* sf, const sir_update_config_data* data) {
    bool retval = _sirfile_validate(sf);

    if (retval) {
        bool updated = false;
        if (_sir_bittest(data->fields, SIRU_LEVELS)) {
            if (sf->levels != *data->levels) {
                _sir_selflog("updating file (id: %"PRIx32") levels from %04"PRIx16
                            " to %04"PRIx16, sf->id, sf->levels, *data->levels);
                sf->levels = *data->levels;
            } else {
                _sir_selflog("skipped superfluous update of file (id: %"PRIx32")"
                            " levels: %04"PRIx16, sf->id, sf->levels);
            }

            updated = true;
        }

        if (_sir_bittest(data->fields, SIRU_OPTIONS)) {
            if (sf->opts != *data->opts) {
                _sir_selflog("updating file (id: %"PRIx32") options from %08"PRIx32
                            " to %08"PRIx32, sf->id, sf->opts, *data->opts);
                sf->opts = *data->opts;
            } else {
                _sir_selflog("skipped superfluous update of file (id: %"PRIx32")"
                            " options: %08"PRIx32, sf->id, sf->opts);
            }

            updated = true;
        }

        retval = updated;
    }

    return retval;
}

sirfileid _sir_fcache_add(sirfcache* sfc, const char* path, sir_levels levels,
    sir_options opts) {
    if (!_sir_validptr(sfc) || !_sir_validstr(path) || !_sir_validlevels(levels) ||
        !_sir_validopts(opts))
        return 0U;

    if (sfc->count >= SIR_MAXFILES)
        return _sir_seterror(_SIR_E_NOROOM);

    const sirfile* existing = _sir_fcache_find(sfc, (const void*)path, _sir_fcache_pred_path);
    if (NULL != existing) {
        _sir_selflog("error: already have file (path: '%s', id: %"PRIx32")",
            path, existing->id);
        return _sir_seterror(_SIR_E_DUPITEM);
    }

    sirfile* sf = _sirfile_create(path, levels, opts);
    if (_sirfile_validate(sf)) {
        _sir_selflog("adding file (path: '%s', id: %"PRIx32"); count = %zu", //-V522
            sf->path, sf->id, sfc->count + 1);

        sfc->files[sfc->count++] = sf;

        if (!_sir_bittest(sf->opts, SIRO_NOHDR) && !_sirfile_writeheader(sf, SIR_FHBEGIN))
            _sir_selflog("warning: failed to write file header (path: '%s', id: %"PRIx32")",
                sf->path, sf->id);

        return sf->id;
    }

    _sir_safefree(&sf);

    return 0U;
}

bool _sir_fcache_update(const sirfcache* sfc, sirfileid id, const sir_update_config_data* data) {
    bool retval = _sir_validptr(sfc) && _sir_validfileid(id) && _sir_validupdatedata(data);

    if (retval) {
        sirfile* found = _sir_fcache_find(sfc, (const void*)&id, _sir_fcache_pred_id);
        retval = found ? _sirfile_update(found, data) : _sir_seterror(_SIR_E_NOITEM);
    }

    return retval;
}

bool _sir_fcache_rem(sirfcache* sfc, sirfileid id) {
    bool retval = _sir_validptr(sfc) && _sir_validfileid(id);

    if (retval) {
        bool found = false;
        for (size_t n = 0; n < sfc->count; n++) {
            SIR_ASSERT(_sirfile_validate(sfc->files[n]));

            if (sfc->files[n]->id == id) {
                _sir_selflog("removing file (path: '%s', id: %"PRIx32"); count = %zu",
                    sfc->files[n]->path, sfc->files[n]->id, sfc->count - 1);

                _sirfile_destroy(&sfc->files[n]);
                _sir_fcache_shift(sfc, n);

                sfc->count--;
                found = true;
                break;
            }
        }

        if (!found)
            retval = _sir_seterror(_SIR_E_NOITEM);
    }

    return retval;
}

void _sir_fcache_shift(sirfcache* sfc, size_t idx) {
    if (_sir_validptr(sfc) && sfc->count <= SIR_MAXFILES) {
        for (size_t n = idx; n < sfc->count - 1; n++) {
            sfc->files[n] = sfc->files[n + 1];
            sfc->files[n + 1] = NULL;
        }
    }
}

bool _sir_fcache_pred_path(const void* match, const sirfile* iter) {
    const char* path = (const char*)match;
    bool equal       = false;

    _sir_selflog("comparing '%s' == '%s'", path, iter->path);

#if !defined(__WIN__)
    /* if we're able to stat both files then we can use that information for the
     * comparison. otherwise, fall back on comparing the canonical path strings
     * returned by realpath. */
    char resolved1[SIR_MAXPATH] = {0};
    char resolved2[SIR_MAXPATH] = {0};
    struct stat st1             = {0};
    struct stat st2             = {0};

    const char* real1 = realpath(path, resolved1);
    const char* real2 = realpath(iter->path, resolved2);

    SIR_UNUSED(real1);
    SIR_UNUSED(real2);

    if ((!stat(resolved1, &st1) && !stat(resolved2, &st2)) ||
        (!stat(path, &st1) && !stat(iter->path, &st2))) {
        equal = st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino;
        _sir_selflog("returning %d for '%s' == '%s'", equal, path, iter->path);
    } else {
        _sir_selflog("falling back to canonical path string compare");
        equal = 0 == strncmp(resolved1, resolved2, SIR_MAXPATH);
        _sir_selflog("returning %d for '%s' == '%s'", equal, resolved1, resolved2);
    }

    return equal;
#else /* __WIN__ */
    /* open both files (only if they already exist) and compare their
     * filesystem info. failing that, fall back on conversion to canonical path
     * and string comparison. */
    DWORD sh_flags   = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    DWORD open_type  = OPEN_EXISTING;
    DWORD attr_flags = FILE_ATTRIBUTE_NORMAL;
    BY_HANDLE_FILE_INFORMATION fi1 = {0};
    BY_HANDLE_FILE_INFORMATION fi2 = {0};

    HANDLE h1 = CreateFileA(path, 0, sh_flags, NULL, open_type, attr_flags, NULL);
    HANDLE h2 = CreateFileA(iter->path,0, sh_flags, NULL, open_type, attr_flags, NULL);

    if (INVALID_HANDLE_VALUE != h1 && INVALID_HANDLE_VALUE != h2 &&
        GetFileInformationByHandle(h1, &fi1) && GetFileInformationByHandle(h2, &fi2)) {
        equal = fi1.dwVolumeSerialNumber == fi2.dwVolumeSerialNumber &&
                fi1.nFileIndexLow        == fi2.nFileIndexLow        &&
                fi1.nFileIndexHigh       == fi2.nFileIndexHigh;
        _sir_selflog("returning %d for '%s' == '%s'", equal, path, iter->path);
    } else {
        _sir_selflog("falling back to canonical path string compare");
        char resolved1[SIR_MAXPATH] = {0}, resolved2[SIR_MAXPATH] = {0};
        if (GetFullPathNameA(path, SIR_MAXPATH, resolved1, NULL) &&
            GetFullPathNameA(iter->path, SIR_MAXPATH, resolved2, NULL))
            equal = 0 == StrCmpIA(resolved1, resolved2);
        _sir_selflog("returning %d for '%s' == '%s'", equal, resolved1, resolved2);
    }

    if (INVALID_HANDLE_VALUE != h1)
        CloseHandle(h1);

    if (INVALID_HANDLE_VALUE != h2)
        CloseHandle(h2);

    return equal;
#endif
}

bool _sir_fcache_pred_id(const void* match, const sirfile* iter) {
    const sirfileid* id = (const sirfileid*)match;
    return iter->id == *id;
}

sirfile* _sir_fcache_find(const sirfcache* sfc, const void* match, sir_fcache_pred pred) {
    bool valid = _sir_validptr(sfc) && _sir_validptr(match) && _sir_validfnptr(pred);

    if (valid) {
        for (size_t n = 0; n < sfc->count; n++) {
            if (pred(match, sfc->files[n]))
                return sfc->files[n];
        }
    }

    return NULL;
}

bool _sir_fcache_destroy(sirfcache* sfc) {
    bool retval = _sir_validptr(sfc);

    if (retval) {
        while (sfc->count > 0) {
            size_t idx = sfc->count - 1;
            SIR_ASSERT(_sirfile_validate(sfc->files[idx]));
            _sirfile_destroy(&sfc->files[idx]);
            sfc->files[idx] = NULL;
            sfc->count--;
        }

        (void)_sir_explicit_memset(sfc, 0, sizeof(sirfcache));
    }

    return retval;
}

bool _sir_fcache_dispatch(const sirfcache* sfc, sir_level level, sirbuf* buf,
    size_t* dispatched, size_t* wanted) {
    bool retval = _sir_validptr(sfc) && _sir_validlevel(level) &&
                  _sir_validptr(buf) && _sir_validptr(dispatched) &&
                  _sir_validptr(wanted);

    if (retval) {
        const char* wrote    = NULL;
        sir_options lastopts = 0U;

        *dispatched = 0;
        *wanted     = 0;

        for (size_t n = 0; n < sfc->count; n++) {
            SIR_ASSERT(_sirfile_validate(sfc->files[n]));

            if (!_sir_bittest(sfc->files[n]->levels, level)) {
                _sir_selflog("level %04"PRIx16" not set in level mask (%04"PRIx16
                            ") for file (path: '%s', id: %"PRIx32"); skipping",
                            level, sfc->files[n]->levels, sfc->files[n]->path,
                            sfc->files[n]->id);
                continue;
            }

            (*wanted)++;

            if (!wrote || sfc->files[n]->opts != lastopts) {
                wrote = _sir_format(false, sfc->files[n]->opts, buf);
                SIR_ASSERT(wrote);
                lastopts = sfc->files[n]->opts;
            }

            if (wrote && _sirfile_write(sfc->files[n], wrote)) {
                (*dispatched)++;
            } else {
                _sir_selflog("error: write to file (path: '%s', id: %"PRIx32") failed!",
                    sfc->files[n]->path, sfc->files[n]->id);
            }
        }

        retval = (*dispatched == *wanted);
    }

    return retval;
}

void _sir_fflush(FILE* f) {
    if (_sir_validptr(f) && 0 != fflush(f))
        (void)_sir_handleerr(errno);
}
