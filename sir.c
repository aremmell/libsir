/*!
 * \file sir.c
 * \brief Implementation of the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */

#define _POSIX_C_SOURCE 200809L
#include "sir.h"

sirinit  sir_s = {0};
sirfiles sir_fc = {0};
sirbuf   sir_b = {0};

bool sir_init(const sirinit* si) {

    if (!si || !_sir_options_sanity(si))
        return false;

    sir_cleanup();
    memcpy(&sir_s, si, sizeof(sirinit));

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
    _sirbuf_reset(&sir_b);

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
        _sirbuf_reset(&sir_b);

        time_t now;
        time(&now);

        output->timestamp = _sirbuf_reserve(&sir_b, SIR_MAXTIME);
        assert(output->timestamp);
        bool bufferStatus = NULL != output->timestamp;

        if (bufferStatus) {           
            size_t fmt = strftime(output->timestamp, SIR_MAXTIME, validstr(sir_s.timeFmt) ?
                sir_s.timeFmt : SIR_TIMEFORMAT, localtime(&now));
            assert(0 != fmt);

            if (0 == fmt)
                _sir_l("%s: strftime returned 0! check SIR_TIMEFORMAT?", __func__);

//            _sirbuf_seek(&sir_b, SIR_MAXTIME);             
        }

        output->level = _sirbuf_reserve(&sir_b, SIR_MAXLEVEL);
        assert(output->level);
        bufferStatus &= NULL != output->level;

        if (bufferStatus) {
            snprintf(output->level, SIR_MAXLEVEL, "%s", _sir_levelstr(level));
            //_sirbuf_seek(&sir_b, SIR_MAXLEVEL);
        }

        if (validstr(sir_s.processName)) {
            output->name = _sirbuf_reserve(&sir_b, SIR_MAXNAME);
            assert(output->name);
            bufferStatus &= NULL != output->name;

            if (bufferStatus) {
                strncpy(output->name, sir_s.processName, SIR_MAXNAME - 1);
                //_sirbuf_seek(&sir_b, SIR_MAXNAME);
            }
        }

        output->message = _sirbuf_reserve(&sir_b, SIR_MAXMESSAGE);
        assert(output->message);
        bufferStatus &= NULL != output->message;

        if (bufferStatus) {
            int msgfmt = vsnprintf(output->message, SIR_MAXMESSAGE, format, args);
            assert(msgfmt >= 0);

            if (msgfmt < 0)
                _sir_l("%s: vsnprintf returned %d!", __func__, msgfmt);

            //_sirbuf_seek(&sir_b, SIR_MAXMESSAGE);
        }
        
        if (bufferStatus) {
            _sir_l("%s: timestamp: '%s', level: '%s', name: '%s', message: '%s'\n",
                __func__, output->timestamp, output->level, output->name, output->message);
            r = _sir_dispatch(level, output);
        }

        safefree(output);
    }

    return r;
}

void _sir_l(const sirchar_t* format, ...) {
#ifdef DEBUG
    sirchar_t output[SIR_MAXMESSAGE] = {0};
    va_list   args;

    va_start(args, format);
    int print = vsnprintf(output, SIR_MAXMESSAGE, format, args);
    va_end(args);

    assert(print > 0);

    if (print > 0) {
        int put = fputs(output, sir_s.selfOutput ? sir_s.selfOutput : stderr);
        assert(put != EOF);
    }
#else
    format = format;
#endif    
}

bool _sir_destformat(sirbuf* buf, sir_options opts, const siroutput* output) {

    assert(buf);
    assert(output);

    if (buf && output && _sirbuf_reset(buf)) {
        if (!flagtest(opts, SIRO_NOTIME)) {
     
        }
    }

    return false;
}

bool _sir_dispatch(sir_level level, const siroutput* output) {

    bool r = false;

    assert(0 != level);
    assert(output);
    
    if (0 != level && output) {
        r &= _sir_stderr_write("");
        r &= _sir_stdout_write("");
#ifndef SIR_NO_SYSLOG
        r &= _sir_syslog_write("");
#endif
    }

    return r;
}

bool _sir_stderr_write(const sirchar_t* message) {
    return false;
}

bool _sir_stdout_write(const sirchar_t* message) {
    return false;
}

bool _sir_syslog_write(const sirchar_t* message) {
    return false;
}

/* bool _sirbuf_append(sirbuf* buf, const sirchar_t* str) {

    assert(_sirbuf_validate(buf));
    assert(validstr(str));

    if (_sirbuf_validate(buf) && validstr(str)) {
        size_t copy = strnlen(str, SIR_MAXMESSAGE);

        _sir_l("%s: append: %2lu, size: %2lu, used: %2lu (%2d left)\n", __func__,
            copy, buf->size, buf->used, (int)buf->size - (int)buf->used);

        if (_sirbuf_reserve(buf, copy)) {
            sirchar_t* cursor = _sirbuf_cursor(buf);
            assert(cursor);

            if (cursor) {
                memcpy(cursor, str, copy);

                buf->ptr[buf->used + copy - 1] = (sirchar_t)'\0';
                buf->used += copy;

                _sir_l("%s: new size: %2lu, contents: '%s'\n", __func__,
                    buf->size, buf->ptr); 
                return true;
            }
        }
    }

    return false;
} */

bool _sirbuf_resize(sirbuf* buf, size_t bytes) {

    assert(_sirbuf_validate(buf));

    if (_sirbuf_validate(buf)) {
        if (bytes >= buf->size - buf->used) {
            _sir_l("%s: resize by %2lu to %2lu\n", __func__, bytes, buf->size + bytes);

            buf->ptr = (sirchar_t*)realloc(buf->ptr, buf->size + bytes);
            buf->size += bytes;

            return true;
        } else {
            _sir_l("%s: %2lu fits (with %2lu left)\n", __func__, bytes, (int)buf->size - (int)bytes);
            return true;
        }
    }

    return false;
}

bool _sirbuf_seek(sirbuf* buf, size_t bytes) {
    
    assert(_sirbuf_validate(buf));

    if (_sirbuf_validate(buf)) {
        if (buf->size - buf->used > bytes) {
            _sir_l("%s: seek by: %lu to: %lu\n", __func__,
                buf->used, buf->used + bytes);            
            buf->used += bytes;
            return true;
        }
    }

    return false;
}

sirchar_t* _sirbuf_reserve(sirbuf* buf, size_t bytes) {

    assert(_sirbuf_validate(buf));

    if (_sirbuf_validate(buf)) {
        if (_sirbuf_resize(buf, bytes)) {
            sirchar_t* cursor = _sirbuf_cursor(buf);
            _sir_l("%s: old used: %lu, new: %lu, cursor at: %016lX ('%s')\n", __func__,
                buf->used, buf->used + bytes, cursor, cursor);
            buf->used += bytes;
            return cursor;
        }
    }

    return NULL;
}

sirchar_t* _sirbuf_cursor(sirbuf* buf) {

    assert(_sirbuf_validate(buf));
    return _sirbuf_validate(buf) ? buf->ptr + buf->used : NULL;
}

bool _sirbuf_reset(sirbuf* buf) {

    if (buf) {
        if (buf->ptr) {
            memset(buf->ptr, 0, buf->size);
        } else {
            assert(0 == buf->used && 0 == buf->size);
            buf->ptr = (sirchar_t*)calloc(SIR_MINPRINT, sizeof(sirchar_t));
            assert(buf->ptr);

            if (buf->ptr)
                buf->size = SIR_MINPRINT;
        }

        buf->used = 0;
    }

    return NULL != buf->ptr;
}

void _sirbuf_destroy(sirbuf* buf) {

    assert(_sirbuf_validate(buf));

    if (buf) {
        safefree(buf->ptr);
        buf->size = 0;
        buf->used = 0;
    }
}

bool _sirbuf_validate(sirbuf* buf) {

    assert(buf);
    assert(buf->ptr);
    assert(buf->used < buf->size);

    return buf && buf->ptr && (buf->used < buf->size);    
}

sirfile* _sirfile_create(int id, const sirchar_t* path, sir_levels levels, sir_options opts) {

    sirfile* sf = NULL;

    assert(validid(id));
    assert(validlevels(levels));
    assert(validstr(path));

    if (validid(id) && validlevels(levels)) {
        FILE* f = _sir_fopen(path);
        assert(f);

        if (f) {
            sf = (sirfile*)calloc(1, sizeof(sirfile));
            assert(sf);

            if (sf) {
                size_t pathLen = strnlen(path, PATH_MAX);
                sf->path       = (sirchar_t*)calloc(pathLen + 1, sizeof(sirchar_t));
                assert(sf->path);

                if (sf->path) {
                    memcpy(sf->path, path, pathLen * sizeof(sirchar_t));
                    sf->f      = f;
                    sf->levels = levels;
                    sf->opts   = opts;
                    sf->id     = id;

                    _sirfile_writeheader(sf);
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
        size_t writeLen = strnlen(output, SIR_MAXOUTPUT);
        size_t write    = fwrite(output, sizeof(sirchar_t), writeLen, sf->f);

        if (write < writeLen) {
            int err = ferror(sf->f);
            int eof = feof(sf->f);

            assert(0 == err && 0 == eof);

            _sir_l("%s: wrote only %lu/%lu bytes to {%d, '%s'}; ferror: %d, feof: %d\n",
                __func__, write, writeLen, sf->id, sf->path, err, eof);

            /*! \todo
             * If an error occurs on write, consider removing file from targets,
             * or at least attempt to roll the file (out of space?)
             */

            clearerr(sf->f);
        }
    }

    return r;
}

bool _sirfile_writeheader(sirfile* sf) {

    if (_sirfile_validate(sf)) {

    }

    return false;
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
    bool valid = sf && validid(sf->id) && sf->f && validstr(sf->path);
    assert(valid);
    return valid;
}

int _sir_files_add(sirfiles* sfc, const sirchar_t* path, sir_levels levels, sir_options opts) {

    assert(sfc);
    assert(validlevels(levels));
    assert(validstr(path));

    if (sfc && validlevels(levels) && validstr(path)) {
        sirfile* sf = _sirfile_create(sfc->count + 1, path, levels, opts);
        assert(_sirfile_validate(sf));

        if (_sirfile_validate(sf)) {
            sfc->files[sfc->count++] = sf;
            return sf->id;
        }
    }

    return SIR_INVALID;
}

bool _sir_files_rem(sirfiles* sfc, int id) {

    assert(sfc);
    assert(validid(id));
    assert(id < sfc->count);

    if (sfc && validid(id) && id < sfc->count) {
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

    if (sfc && 0 != level && output) {
        size_t written = 0;
        for (size_t n = 0; n < sfc->count; n++) {
            assert(_sirfile_validate(sfc->files[n]));

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

bool _sir_options_sanity(const sirinit* si) { 
    return true;
}

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

bool _sir_destwantslevel(sir_levels destLevels, sir_level level) {
    return flagtest(destLevels, level);
}

/*! \endcond PRIVATE */
