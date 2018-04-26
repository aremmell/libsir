/*!
 * \file sir.c
 * \brief Implementation of the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */
#include "sir.h"
#include <assert.h>
#include <time.h>

#ifndef _WIN32
#include <syslog.h>
#else
#include <Windows.h>
#endif

sirinit  sir_s;
sirfiles sir_fc;
sirbuf   sir_b;

bool sir_init(const sirinit* si) {

    if (!si || !_sir_options_sanity(si))
        return false;

    sir_cleanup();
    memcpy(&sir_s, si, sizeof(sirinit));

    if (!_sirbuf_reset(&sir_b, true))
        return false;

    if (!sir_s.selfOutput)
        sir_s.selfOutput = stderr;

    return true;
}

bool sirdebug(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirinfo(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirnotice(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirwarn(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_WARN, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirerror(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

bool sircrit(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

bool siralert(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

bool siremerg(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

void sir_cleanup(void) {
    _sir_files_destroy(&sir_fc);
    _sirbuf_reset(&sir_b, false);

    memset(&sir_s, 0, sizeof(sirinit));
    memset(&sir_fc, 0, sizeof(sirfiles));
}

int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    return _sir_files_add(&sir_fc, path, levels, opts);
}

bool sir_remfile(int id) { return _sir_files_rem(&sir_fc, id); }

/*! \cond PRIVATE */

bool _sir_lv(sir_level level, const sirchar_t* format, va_list args) {

    bool r = false;

    assert(0 != level);
    assert(validstr(format));

    siroutput* output = (siroutput*)calloc(1, sizeof(siroutput));

    if (output) {

        
        safefree(output);
    }
    /*static sirbuf* buf = NULL;

    if (NULL == buf)
        buf = _sirbuf_create();

    if (NULL != buf) {
        sirchar_t output[SIR_MAXMESSAGE] = {0};
        sirchar_t timestr[SIR_MAXTIME] = {0};

        if (!flagtest(sir_s.opts, SIRO_NOTIME)) {

            time_t now = 0;
            time(&now);
            strftime(timestr, SIR_MAXTIME, validstr(sir_s.timeFmt) ?
    sir_s.timeFmt : SIR_TIMEFORMAT, localtime(&now));

            timestr[strlen(timestr) - 1] = _T('\0');
            _sirbuf_append(buf, timestr);
        }

        if (!flagtest(sir_s.opts, SIRO_NOLEVEL)) {
            sirchar_t levelstr[16] = {0};
            snprintf(levelstr, 16, " [%s]", _sir_levelstr(levels));
            _sirbuf_append(buf, levelstr);
        }

        if (!flagtest(sir_s.opts, SIRO_NONAME)) {
            _sirbuf_append(buf, " sups");
        } else {
            _sirbuf_append(buf, 0 == buf->size ? ": " : ": ");
        }/

        vsnprintf(output, SIR_MAXMESSAGE, format, args);

        if (0 == _sirbuf_append(buf, output) /*&& 0 == _sirbuf_append(buf,
    SIR_LINEENDING)/) r = _sir_dispatch(buf->ptr, levels);

        buf->used = 0;
        buf->ptr[0] = '\0';
    }*/

    return r;
}

bool _sir_l(const sirchar_t* format, ...) {
    sirchar_t output[SIR_MAXMESSAGE] = {0};
    va_list   args;

    va_start(args, format);
    int print = vsnprintf(output, SIR_MAXMESSAGE, format, args);
    va_end(args);

    if (print > 0) {
        int put = fputs(output, sir_s.selfOutput ? sir_s.selfOutput : stderr);
        return put != EOF;
    }

    return false;
}

bool _sir_dispatch(sir_level level, const siroutput* output) {

    bool r = false;

    // TODO:
    /*if (!flagtest(sir_s.opts, SIRO_NOSTDERR) &&
    flagtest(sir_s.stdErrLevels, levels)) fputs(output, stderr);

    if (!flagtest(sir_s.opts, SIRO_NOSTDOUT) && flagtest(sir_s.stdOutLevels,
    levels)) fputs(output, stdout);

    if (!flagtest(sir_s.opts, SIRO_NOSYSLOG) && flagtest(sir_s.sysLogLevels,
    levels)) {
        /* TODO: syslog. /
    }

    _sir_files_dispatch(&sir_fc, output, levels);

    r = true;*/

    return r;
}

bool _sirbuf_append(sirbuf* buf, const sirchar_t* str) {

    bool r = false;

    assert(buf);
    assert(buf->ptr);
    assert(validstr(str));

    if (buf && buf->ptr && validstr(str)) {

        /* _sir_l("%s: ptr: %16lX, size: %2lu, used: %2lu (%2d left)\n",
            __FUNCTION__, (uint64_t)buf->ptr, buf->size, buf->used, (int)buf->size - (int)buf->used); */

        size_t copy = (strlen(str) + 1) * sizeof(sirchar_t);

        if (copy >= buf->size - buf->used) {
            /* _sir_l(
                "%s: ptr: %16lX, resize by %2lu to %2lu\n",
                __FUNCTION__, (uint64_t)buf->ptr, copy, buf->size + copy); */

            buf->ptr = (sirchar_t*)realloc(buf->ptr, buf->size + copy);
            buf->size += copy;
        } /* else {
            _sir_l("%s: ptr: %16lX, %2lu fits (with %2lu left)\n", __FUNCTION__, (uint64_t)buf->ptr, copy,
                (int)buf->size - copy);
        } */

        if (buf->ptr) {
            memcpy(buf->ptr + buf->used, str, copy);
            buf->ptr[buf->used + copy - 1] = '\0';
            buf->used += copy;
            /* _sir_l("%s: ptr: %16lX, new size: %2lu, contents: %s\n", __FUNCTION__, (uint64_t)buf->ptr, buf->size,
                buf->ptr); */
            r = true;
        }
    }

    return r;
}

bool _sirbuf_reset(sirbuf* buf, bool realloc) {

    assert(buf);

    if (buf) {
        safefree(buf->ptr);

        if (realloc) {
            buf->ptr = (sirchar_t*)calloc(SIR_MINPRINT, sizeof(sirchar_t));
        } else {
            assert(buf->ptr);

            if (buf->ptr)
                buf->ptr[0] = (sirchar_t)'\0';
        }

        buf->size = buf->ptr ? SIR_MINPRINT : 0;
        buf->used = 0;
    }

    assert(realloc ? NULL != buf->ptr : NULL != buf);
    return realloc ? NULL != buf->ptr : NULL != buf;
}

sirfile* _sirfile_create(int id, const sirchar_t* path, sir_levels levels, sir_options opts) {

    sirfile* sf = NULL;

    assert(0 != id);
    assert(0 != levels);
    assert(validstr(path));

    if (0 != id && 0 != levels) {
        FILE* f = _sir_fopen(path);

        assert(f);

        if (f) {
            sf = (sirfile*)calloc(1, sizeof(sirfile));

            assert(sf);

            if (sf) {
                size_t pathLen = strlen(path);
                sf->path       = (sirchar_t*)calloc(pathLen + 1, sizeof(sirchar_t));

                assert(sf->path);

                if (sf->path) {
                    memcpy(sf->path, path, pathLen * sizeof(sirchar_t));
                    sf->f      = f;
                    sf->levels = levels;
                    sf->opts   = opts;
                    sf->id     = id;
                }
            }

            if (f && (!sf || !sf->path))
                _sir_fclose(&f);
        }
    }

    return sf;
}

bool _sirfile_write(sirfile* sf, const sirchar_t* output) {

    bool r = false;

    assert(validstr(output));

    if (_sirfile_validate(sf) && validstr(output)) {
        size_t writeLen = strlen(output);
        size_t write    = fwrite(output, sizeof(sirchar_t), writeLen, sf->f);

        if (write < writeLen) {
            int err = ferror(sf->f);
            int eof = feof(sf->f);

            assert(0 == err && 0 == eof);

            _sir_l("%s: wrote only %lu/%lu bytes to {%d, '%s'}; ferror: %d, feof: %d\n",
                __FUNCTION__, write, writeLen, sf->id, sf->path, err, eof);

            /*! \todo
             * If an error occurs on write, consider removing file from targets,
             * or at least attempt to roll the file (out of space?)
             */

            clearerr(sf->f);
        }
    }

    return r;
}

FILE* _sir_fopen(const sirchar_t* path) { return validstr(path) ? fopen(path, "a") : NULL; }

void _sir_fclose(FILE** f) {
    if (f && *f)
        safefclose(*f);
}

void _sirfile_destroy(sirfile* sf) {
    if (sf) {
        _sir_fclose(&sf->f);
        safefree(sf->path);
        safefree(sf);
    }
}

bool _sirfile_validate(sirfile* sf) {
    bool valid = sf && 0 != sf->id && sf->f && validstr(sf->path);
    assert(valid);
    return valid;
}

bool _sir_files_add(sirfiles* sfc, const sirchar_t* path, sir_levels levels, sir_options opts) {

    assert(sfc);
    assert(validstr(path));
    assert(0 != levels);

    if (sfc && 0 != levels && validstr(path)) {
        sirfile* sf = _sirfile_create(sfc->count, path, levels, opts);

        assert(sf);

        if (sf) {
            sfc->files[sfc->count++] = sf;
            return true;
        }
    }

    return false;
}

bool _sir_files_rem(sirfiles* sfc, int id) {

    assert(sfc);
    assert(0 != id);
    assert(id < sfc->count);

    if (sfc && 0 != id && id < sfc->count) {
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

            if (sfc->files[n]->id == id) {
                _sirfile_destroy(sfc->files[n]);
                for (size_t i = n; i < sfc->count - 1; i++) {
                    sfc->files[i]     = sfc->files[i + 1];
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

bool _sir_files_dispatch(sirfiles* sfc, sir_level level, const siroutput* output) {

    assert(sfc);
    assert(0 != level);
    assert(output);

    if (sfc && output) {
        size_t written = 0;
        for (size_t n = 0; n < sfc->count; n++) {
            if (!_sir_destwantslevel(sfc->files[n]->levels, level))
                continue;

            /*! \todo format string and write */
            _sirfile_write(sfc->files[n], "");
            written++;
        }

        return 0 != written;
    }

    return false;
}

bool _sir_options_sanity(const sirinit* si) { return true; }

const sirchar_t* _sir_levelstr(sir_level level) {
    switch (level) {
        case SIRL_EMERG: return SIRL_S_EMERG;
        case SIRL_ALERT: return SIRL_S_ALERT;
        case SIRL_CRIT: return SIRL_S_CRIT;
        case SIRL_ERROR: return SIRL_S_ERROR;
        case SIRL_WARN: return SIRL_S_WARN;
        case SIRL_NOTICE: return SIRL_S_NOTICE;
        case SIRL_INFO: return SIRL_S_INFO;
        case SIRL_DEBUG:
        default: return SIRL_S_DEBUG;
    }
}

bool _sir_destwantslevels(sir_levels destLevels, sir_level level) {
    return flagtest(destLevels, level);
}

/*! \endcond PRIVATE */
