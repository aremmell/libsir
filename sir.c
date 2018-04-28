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

sirinit  sir_s  = {0};
sirfiles sir_fc = {0};
sirbuf   sir_b  = {0};

bool sir_init(const sirinit* si) {

    if (!si || !_sir_options_sanity(si))
        return false;

    sir_cleanup();
    memcpy(&sir_s, si, sizeof(sirinit));

    if (!sir_s.selfOutput)
        sir_s.selfOutput = stderr;

#ifndef SIR_NO_SYSLOG
// TODO: if not using process name, use pid for syslog identity?
    if (0 != sir_s.sysLogLevels)
        openlog(validstr(sir_s.processName) ? sir_s.processName : "",
            (sir_s.sysLogIncludePID ? LOG_PID : 0) | LOG_ODELAY, LOG_USER);
#endif

#ifdef _WIN32
#ifdef DEBUG
    _set_invalid_parameter_handler(_sir_invalidparam);
#endif
#endif

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
    memset(&sir_s, 0, sizeof(sirinit));
    memset(&sir_fc, 0, sizeof(sirfiles));
    _sirbuf_reset(&sir_b);
}

int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    return _sir_files_add(&sir_fc, path, levels, opts);
}

bool sir_remfile(int id) {
    return _sir_files_rem(&sir_fc, id);
}

/*! \cond PRIVATE */

bool _sir_lv(sir_level level, const sirchar_t* format, va_list args) {

    assert(0 != level);
    assert(validstr(format));

    siroutput output = {0};

    output.timestamp = _sirbuf_get(&sir_b, _SIRBUF_TIME);
    assert(output.timestamp);

    time_t now;
    long nowmsec;
    bool gettime = _sir_getlocaltime(&now, &nowmsec);
    assert(gettime);

    if (gettime) {
        const sirchar_t* timeformat = validstr(sir_s.timeFmt) ? sir_s.timeFmt : SIR_TIMEFORMAT;
        bool fmt = _sir_formattime(now, output.timestamp, timeformat);
        assert(fmt);

        output.msec = _sirbuf_get(&sir_b, _SIRBUF_MSEC);
        assert(output.msec);

        snprintf(output.msec, SIR_MAXMSEC, SIR_MSECFORMAT, nowmsec);
    }

    output.level = _sirbuf_get(&sir_b, _SIRBUF_LEVEL);
    assert(output.level);
    snprintf(output.level, SIR_MAXLEVEL, "[%s]", _sir_levelstr(level));

    if (validstr(sir_s.processName)) {
        output.name = _sirbuf_get(&sir_b, _SIRBUF_NAME);
        assert(output.name);
        strncpy(output.name, sir_s.processName, SIR_MAXNAME - 1);
    }

    /*! \todo add support for syslog's %m */
    output.message = _sirbuf_get(&sir_b, _SIRBUF_MSG);
    assert(output.message);
    int msgfmt = vsnprintf(output.message, SIR_MAXMESSAGE, format, args);
    assert(msgfmt >= 0);

    if (msgfmt < 0)
        _sir_l("%s: vsnprintf returned %d!", __func__, msgfmt);

    output.output = _sirbuf_get(&sir_b, _SIRBUF_OUTPUT);
    assert(output.output);

    if (output.output)
        output.output[0] = (sirchar_t)'\0';

    return _sir_dispatch(level, &output);
}

void _sir_l(const sirchar_t* format, ...) {
#ifdef SIR_SELFLOG
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

bool _sir_dispatch(sir_level level, siroutput* output) {

    bool r = true;

    assert(0 != level);
    assert(output);

    if (0 != level && output) {
        if (_sir_destwantslevel(sir_s.stdErrLevels, level)) {
            const sirchar_t* write = _sir_format(sir_s.stdErrOptions, output);
            assert(write);
            r &= NULL != write && _sir_stderr_write(write);
        }

        if (_sir_destwantslevel(sir_s.stdOutLevels, level)) {
            const sirchar_t* write = _sir_format(sir_s.stdOutOptions, output);
            assert(write);
            r &= _sir_stdout_write(output->output);
        }

#ifndef SIR_NO_SYSLOG
        if (_sir_destwantslevel(sir_s.sysLogLevels, level)) {
            syslog(_sir_syslog_maplevel(level), "%s", output->message);
        }
#endif

        return r && _sir_files_dispatch(&sir_fc, level, output);
    }

    return false;
}

const sirchar_t* _sir_format(sir_options opts, siroutput* output) {

    assert(validopts(opts));
    assert(output);
    assert(output->output);

    /*
     * [time stamp][msec] [level] [name]: message
     */

    if (validopts(opts) && output && output->output) {
        bool first = true;

        if (!flagtest(opts, SIRO_NOTIME)) {
            strncat(output->output, output->timestamp, SIR_MAXTIME);
            first = false;

#ifdef SIR_MSEC_TIMER
            if (!flagtest(opts, SIRO_NOMSEC))
                strncat(output->output, output->msec, SIR_MAXMSEC);
#endif                            
        }

        if (!flagtest(opts, SIRO_NOLEVEL)) {
            if (!first)
                strncat(output->output, " ", 1);
            strncat(output->output, output->level, SIR_MAXLEVEL);
            first = false;
        }

        if (!flagtest(opts, SIRO_NONAME)) {
            if (!first)
                strncat(output->output, " ", 1);
            strncat(output->output, output->name, SIR_MAXNAME);
            first = false;
        }

        if (!first)
            strncat(output->output, ": ", 2);

        strncat(output->output, output->message, SIR_MAXMESSAGE);
        strncat(output->output, SIR_LINEENDING, 2);

        return output->output;
    }

    return NULL;
}

bool _sir_stderr_write(const sirchar_t* message) {
    int write = fputs(message, stderr);
    assert(write > 0);
    return write > 0;
}

bool _sir_stdout_write(const sirchar_t* message) {
    int write = puts(message);
    assert(write >= 0);
    return write >= 0;
}

#ifndef SIR_NO_SYSLOG
int _sir_syslog_maplevel(sir_level level) {
    switch (level) {
        case SIRL_EMERG: return LOG_EMERG;
        case SIRL_ALERT: return LOG_ALERT;
        case SIRL_CRIT: return LOG_CRIT;
        case SIRL_ERROR: return LOG_ERR;
        case SIRL_WARN: return LOG_WARNING;
        case SIRL_NOTICE: return LOG_NOTICE;
        case SIRL_INFO: return LOG_INFO;
        case SIRL_DEBUG: return LOG_DEBUG;
        default: assert(false); return LOG_INFO;
    }
}
#endif

void _sirbuf_reset(sirbuf* buf) {
    if (buf)
        memset(buf, 0, sizeof(sirbuf));
}

/* in case there's a better way to implement this
 * hms, abstract it away.
 */
sirchar_t* _sirbuf_get(sirbuf* buf, size_t idx) {

    assert(idx <= _SIRBUF_MAX);

    switch (idx) {
        case _SIRBUF_TIME: return buf->timestamp;
        case _SIRBUF_MSEC: return buf->msec;
        case _SIRBUF_LEVEL: return buf->level;
        case _SIRBUF_NAME: return buf->name;
        case _SIRBUF_MSG: return buf->message;
        case _SIRBUF_OUTPUT: return buf->output;
        default: assert(false);
    }

    return NULL;
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

            _sir_l("%s: wrote %lu/%lu bytes to %d; ferror: %d, feof: %d\n", __func__, write, writeLen,
                sf->id, err, eof);

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
        bool gettime = _sir_getlocaltime(&now, NULL);
        assert(gettime);

        if (gettime) {
            sirchar_t time[SIR_MAXTIME] = {0};      
            bool fmttime = _sir_formattime(now, time, SIR_FHTIMEFORMAT);
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
                _sir_l("%s: levels for %d (%04lx) not set for (%04lx); skipping...\n",
                    __func__, sfc->files[n]->id, sfc->files[n]->levels, level, written, sfc->count);
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

        _sir_l("%s: wrote to %d/%lu log file(s)\n", __func__, written, sfc->count);
    }

    return r;
}

bool _sir_options_sanity(const sirinit* si) {
    return true;
}

const sirchar_t* _sir_levelstr(sir_level level) {
    switch (level) {
        case SIRL_INFO: return SIRL_S_INFO;
        case SIRL_NOTICE: return SIRL_S_NOTICE;
        case SIRL_WARN: return SIRL_S_WARN;
        case SIRL_ERROR: return SIRL_S_ERROR;
        case SIRL_CRIT: return SIRL_S_CRIT;
        case SIRL_ALERT: return SIRL_S_ALERT;
        case SIRL_EMERG: return SIRL_S_EMERG;
        case SIRL_DEBUG:
        default: return SIRL_S_DEBUG;
    }
}

bool _sir_destwantslevel(sir_levels destLevels, sir_level level) {
    return flagtest(destLevels, level);
}

bool _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format) {

    assert(now);
    assert(buffer);
    assert(validstr(format));

    if (0 != now && buffer && validstr(format)) {
        size_t fmt = strftime(buffer, SIR_MAXTIME, format, localtime(&now));
        assert(0 != fmt);

        if (0 == fmt)
            _sir_l("%s: strftime returned 0; format string: '%s'", __func__, format);
    
        return 0 != fmt;
    }

    return false;
}

bool _sir_getlocaltime(time_t* tbuf, long* nsecbuf) {

    assert(tbuf);

    if (tbuf) {
#ifdef SIR_MSEC_TIMER
        struct timespec ts = {0};

        int clock = clock_gettime(SIR_MSECCLOCK, &ts);
        assert(0 == clock);

        if (0 == clock) {
            *tbuf = ts.tv_sec;
            if (nsecbuf) {
                *nsecbuf = (ts.tv_nsec / 1e6);
                assert(*nsecbuf < 1000);
            }
        } else {
            time(tbuf);
            *nsecbuf = 0;
            _sir_l("%s: clock_gettime failed; errno: %d\n", __func__, errno);
        }
#else
#pragma message "no support for millisecond computation on this platform."
        time(tbuf);

        if (nsecbuf)
            *nsecbuf = 0;
#endif        
        return true;
    }

    return false;
}

#if defined(_WIN32) && defined(DEBUG)
    void _sir_invalidparam(
        const wchar_t * expression,
        const wchar_t * function,
        const wchar_t * file,
        unsigned int line,
        uintptr_t pReserved
    ) {
        const char* format = "%s: invalid paramter handler called:\n{\n\texpression: %S\n\tfunc: %S\n\tfile: %S\n\tline: %d";
#ifndef SIR_SELFLOG
        fprintf(stderr, format, __func__, expression, function, file, line);
#else
        _sir_l(format, __func__, expression, function, file, line);
#endif
        abort();
    }
#endif

/*! \endcond PRIVATE */
