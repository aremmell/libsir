/*!
 * \file sirfilecache.c
 *
 * Internal implementation of log file management for the SIR library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sirfilecache.h"
#include "sirinternal.h"
#include "sirmacros.h"

/*! \cond PRIVATE */

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
            validid(fd);

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
        size_t writeLen = strnlen(output, SIR_MAXOUTPUT);
        size_t write    = fwrite(output, sizeof(sirchar_t), writeLen, sf->f);

        assert(write == writeLen);

        if (write < writeLen) {
            int err = ferror(sf->f);
            int eof = feof(sf->f);

            assert(0 == err && 0 == eof);

            _sir_l("%s: wrote %lu/%lu bytes to %d; ferror: %d, feof: %d\n", __func__, write, writeLen, sf->id,
                err, eof);

            /*! \todo
             * If an error occurs on write, consider removing file from targets,
             * or at least attempt to roll the file (out of space?)
             */

            clearerr(sf->f);
        }

        return write == writeLen;
    }

    return false;
}

bool _sirfile_writeheader(sirfile* sf) {

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

                int fmt = snprintf(header, SIR_MAXOUTPUT, SIR_FHFORMAT, time);
                assert(fmt >= 0);

                if (fmt < 0) {
                    _sir_l("%s: snprintf returned %d!", __func__, fmt);
                } else {
                    return _sirfile_write(sf, header);
                }
            }
        }
    }

    return false;
}

FILE* _sir_fopen(const sirchar_t* path) {
    return validstr(path) ? fopen(path, "a") : NULL;
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
        assert(0 == flush);
    }
}

bool _sir_fflush_all() {
    int flush = fflush(NULL);
    assert(0 == flush);
    return 0 == flush;
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

int _sir_files_add(sirfiles* sfc, const sirchar_t* path, sir_levels levels, sir_options opts) {

    assert(sfc);
    assert(validstr(path));
    assert(validlevels(levels));
    assert(validopts(opts));

    if (sfc && validlevels(levels) && validopts(opts) && validstr(path)) {
        assert(sfc->count < SIR_MAXFILES);

        if (sfc->count < SIR_MAXFILES) {
            sirfile* sf = _sirfile_create(path, levels, opts);
            assert(_sirfile_validate(sf));

            if (_sirfile_validate(sf)) {
                sfc->files[sfc->count++] = sf;

                if (!flagtest(sf->opts, SIRO_NOHDR))
                    _sirfile_writeheader(sf);

                return sf->id;
            }
        }
    }

    return SIR_INVALID;
}

bool _sir_files_rem(sirfiles* sfc, int id) {

    assert(sfc);
    assert(validid(id));

    if (sfc && validid(id)) {
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (sfc->files[n]->id == id) {
                _sirfile_destroy(sfc->files[n]);

                for (size_t i = n; i < sfc->count; i++) {
                    sfc->files[i] = sfc->files[i + 1];

                    if (i < sfc->count - 1)
                        sfc->files[i + 1] = NULL;

                    assert(sfc->count > 0);
                    sfc->count--;
                }

                return true;
            }
        }
    }

    return false;
}

bool _sir_files_destroy(sirfiles* sfc) {

    assert(sfc);

    if (sfc) {
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));
            _sirfile_destroy(sfc->files[n]);
            sfc->files[n] = NULL;
            assert(sfc->count > 0);
            sfc->count--;
        }

        return true;
    }

    return false;
}

bool _sir_files_dispatch(sirfiles* sfc, sir_level level, siroutput* output) {

    bool r = true;

    assert(sfc);
    assert(0 != level);
    assert(output);

    if (sfc && 0 != level && output) {
        size_t written = 0;
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (!_sir_destwantslevel(sfc->files[n]->levels, level)) {
                _sir_l("%s: levels for %d (%04lx) not set for (%04lx); skipping...\n", __func__,
                    sfc->files[n]->id, sfc->files[n]->levels, level, written, sfc->count);
                continue;
            }

            const sirchar_t* write = _sir_format(sfc->files[n]->opts, output);
            assert(write);

            if (write && _sirfile_write(sfc->files[n], write)) {
                r &= true;
                written++;
            } else {
                _sir_l("%s: write to %d failed! errno: %d\n", __func__, sfc->files[n]->id, errno);
            }
        }

        if (written > 0) {
            if (!_sir_fflush_all())
                _sir_l("%s: fflush failed! errno: %d\n", __func__, errno);
        }

        if (sfc->count > 0)
            _sir_l("%s: wrote to %d/%lu log file(s)\n", __func__, written, sfc->count);
    }

    return r;
}

/*! \endcond */
