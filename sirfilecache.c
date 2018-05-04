/**
 * @file sirfilecache.c
 * @brief Log file management.
 */
#include "sirfilecache.h"
#include "sirinternal.h"
#include "sirmutex.h"

/**
 * @addtogroup intern
 * @{
 */

int _sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {

    if (_sir_sanity()) {
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);

        if (sfc) {
            int r = _sir_fcache_add(sfc, path, levels, opts);
            _sir_unlocksection(_SIRM_FILECACHE);
            return r;
        }
    }

    return SIR_INVALID;
}

bool _sir_remfile(int id) {

    if (_sir_sanity()) {
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);

        if (sfc) {
            bool r = _sir_fcache_rem(sfc, id);
            return _sir_unlocksection(_SIRM_FILECACHE) && r;
        }
    }

    return false;
}

sirfile* _sirfile_create(const sirchar_t* path, sir_levels levels, sir_options opts) {

    sirfile* sf = NULL;

    assert(validstr(path));
    assert(validlevels(levels));
    assert(validopts(opts));

    if (validstr(path) && validlevels(levels) && validopts(opts)) {
        FILE* f = _sir_fopen(path);
        assert(f);

        if (f) {
            int fd = fileno(f);
            if (-1 == fd) _sir_handleerr(errno);

            if (validid(fd)) {
                sf = (sirfile*)calloc(1, sizeof(sirfile));
                assert(sf);

                if (sf) {
                    size_t pathLen = strnlen(path, SIR_MAXPATH);
                    sf->path       = (sirchar_t*)calloc(pathLen + 1, sizeof(sirchar_t));
                    assert(sf->path);

                    if (sf->path) {
                        strncpy(sf->path, path, pathLen);
                        sf->f      = f;
                        sf->levels = levels;
                        sf->opts   = opts;
                        sf->id     = fd;
                    }
                }
            }

            if (f && (!sf || !sf->path))
                _sir_fclose(&f);
        }
    }

    return sf;
}

bool _sirfile_write(sirfile* sf, const sirchar_t* output) {

    assert(validstr(output));

    if (_sirfile_validate(sf) && validstr(output)) {

        if (_sirfile_needsroll(sf)) {
            if (!_sirfile_roll(sf))
                return false;

            if (!_sirfile_writeheader(sf, SIR_FHROLLED))
                return false;
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

            /** \todo
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

    assert(_sirfile_validate(sf));

    if (_sirfile_validate(sf)) {
        time_t now;
        bool   gettime = _sir_getlocaltime(&now, NULL);
        assert(gettime);

        if (gettime) {
            sirchar_t time[SIR_MAXTIME] = {0};
            bool      fmttime           = _sir_formattime(now, time, SIR_FHTIMEFORMAT);
            assert(fmttime);

            if (fmttime) {
                sirchar_t header[SIR_MAXOUTPUT] = {0};

                int fmt = snprintf(header, SIR_MAXOUTPUT, SIR_FHFORMAT, msg, time);
                assert(fmt >= 0);

                if (fmt < 0) {
                    _sir_selflog("%s: snprintf returned %d!", __func__, fmt);
                } else {
                    return _sirfile_write(sf, header);
                }
            }
        }
    }

    return false;
}

bool _sirfile_needsroll(sirfile* sf) {

    assert(_sirfile_validate(sf));

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

bool _sirfile_roll(sirfile* sf) {

    assert(_sirfile_validate(sf));

    if (_sirfile_validate(sf)) {
#ifndef _WIN32
        int roll = ftruncate(sf->id, 0);
#else
        int roll = _chsize(sf->id, 0);
#endif
        assert(0 == roll);

        if (0 != roll) {
            _sir_handleerr(errno);
            return false;
        }

        _sir_selflog("%s: rolled '%s'\n", __func__, sf->path);
        return true;
    }

    return false;
}

void _sirfile_destroy(sirfile* sf) {
    if (sf) {
        _sir_fflush(sf->f);
        _sir_fclose(&sf->f);
        safefree(sf->path);
        safefree(sf);
    }
}

bool _sirfile_validate(sirfile* sf) {
    bool valid = sf && validid(sf->id) && sf->f && validstr(sf->path);
    assert(valid);
    return valid;
}

int _sir_fcache_add(sirfcache* sfc, const sirchar_t* path, sir_levels levels, sir_options opts) {

    assert(sfc);
    assert(validstr(path));
    assert(validlevels(levels));
    assert(validopts(opts));

    if (sfc && validstr(path) && validlevels(levels) && validopts(opts)) {

        sirfile* existing = _sir_fcache_find(sfc, (const void*)path, _sir_fcache_pred_path);

        if (NULL != existing) {
            _sir_selflog("%s: file with path '%s' already added.\n", __func__, path);
            return SIR_INVALID;
        }

        assert(sfc->count < SIR_MAXFILES);

        if (sfc->count < SIR_MAXFILES) {
            sirfile* sf = _sirfile_create(path, levels, opts);
            assert(_sirfile_validate(sf));

            if (_sirfile_validate(sf)) {
                sfc->files[sfc->count++] = sf;

                if (!flagtest(sf->opts, SIRO_NOHDR))
                    _sirfile_writeheader(sf, SIR_FHBEGIN);

                return sf->id;
            }
        }
    }

    return SIR_INVALID;
}

bool _sir_fcache_rem(sirfcache* sfc, int id) {

    assert(sfc);
    assert(validid(id));

    if (sfc && validid(id)) {
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (sfc->files[n]->id == id) {
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

sirfile* _sir_fcache_find(sirfcache* sfc, const void* match, sir_fcache_pred pred) {
    assert(sfc);
    assert(match);
    assert(pred);

    if (sfc && match && pred) {
        for (size_t n = 0; n < sfc->count; n++) {
            if (pred(match, sfc->files[n]))
                return sfc->files[n];
        }
    }

    return NULL;
}

bool _sir_fcache_destroy(sirfcache* sfc) {

    assert(sfc);

    if (sfc) {
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

bool _sir_fcache_dispatch(sirfcache* sfc, sir_level level, siroutput* output, size_t* dispatched) {

    bool r = true;

    assert(sfc);
    assert(validlevel(level));
    assert(output);
    assert(dispatched);

    if (sfc && validlevel(level) && output && dispatched) {
        *dispatched = 0;
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (!_sir_destwantslevel(sfc->files[n]->levels, level)) {
                _sir_selflog("%s: levels for %d (%04lx) not set for (%04lx); skipping...\n", __func__,
                    sfc->files[n]->id, sfc->files[n]->levels, level);
                continue;
            }

            const sirchar_t* write = _sir_format(false, sfc->files[n]->opts, output);
            assert(write);

            if (write && _sirfile_write(sfc->files[n], write)) {
                r &= true;
                (*dispatched)++;
            } else {
                _sir_selflog("%s: write to %d failed!\n", __func__, sfc->files[n]->id);
            }
        }

        if (*dispatched > 0) {
            if (!_sir_fflush_all())
                _sir_selflog("%s: fflush failed! errno: %d\n", __func__, errno);
        }
    }

    return r;
}

FILE* _sir_fopen(const sirchar_t* path) {
    if (validstr(path)) {
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
    if (f && *f) {
        safefclose(*f);
    }
}

void _sir_fflush(FILE* f) {

    assert(f);

    if (f) {
        int flush = fflush(f);
        if (0 != flush) _sir_handleerr(errno);
    }
}

bool _sir_fflush_all() {
    int flush = fflush(NULL);
    if (0 != flush) _sir_handleerr(errno);
    return 0 == flush;
}

/** @} */
