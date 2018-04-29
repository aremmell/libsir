/*!
 * \file sirinternal.c
 * \brief Core internal implementation of the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sirinternal.h"
#include "sirfilecache.h"

/*! \cond PRIVATE */

sirinit  sir_s  = {0};
sirfiles sir_fc = {0};
sirbuf   sir_b  = {0};
uint32_t sir_magic = 0;

bool _sir_lv(sir_level level, const sirchar_t* format, va_list args) {

    assert(0 != level);
    assert(validstr(format));

    siroutput output = {0};

    output.timestamp = _sirbuf_get(&sir_b, _SIRBUF_TIME);
    assert(output.timestamp);

    time_t now;
    long   nowmsec;
    bool   gettime = _sir_getlocaltime(&now, &nowmsec);
    assert(gettime);

    if (gettime) {
        const sirchar_t* timeformat = validstr(sir_s.timeFmt) ? sir_s.timeFmt : SIR_TIMEFORMAT;
        bool             fmt        = _sir_formattime(now, output.timestamp, timeformat);
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
#pragma message "TODO: PID/TID"
        //snprintf(output.name, SIR_MAXNAME, "%s (%d:%d)", sir_s.processName, _sir_getpid(), _sir_gettid());
    }

    /*! \todo add support for syslog's %m */
    output.message = _sirbuf_get(&sir_b, _SIRBUF_MSG);
    assert(output.message);
    int msgfmt = vsnprintf(output.message, SIR_MAXMESSAGE, format, args);
    assert(msgfmt >= 0);

    if (msgfmt < 0)
        _sir_selflog("%s: vsnprintf returned %d!", __func__, msgfmt);

    output.output = _sirbuf_get(&sir_b, _SIRBUF_OUTPUT);
    assert(output.output);

    return _sir_dispatch(level, &output);
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

        resetstr(output->output);

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
        strncat(output->output, "\n", 1);

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
    int write = fputs(message, stdout);
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

    assert(buf);

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
            _sir_selflog("%s: strftime returned 0; format string: '%s'", __func__, format);

        return 0 != fmt;
    }

    return false;
}

bool _sir_getlocaltime(time_t* tbuf, long* nsecbuf) {
    if (tbuf) {
#ifdef SIR_MSEC_POSIX
        struct timespec ts = {0};
        int clock = clock_gettime(SIR_MSECCLOCK, &ts);
        assert(0 == clock);

        if (0 == clock) {
            if (nsecbuf) {
                *nsecbuf = (ts.tv_nsec / 1e6);
                assert(*nsecbuf < 1000);
            }
        } else {
            *nsecbuf = 0;
            _sir_selflog("%s: clock_gettime failed; errno: %d\n", __func__, errno);
        }
#elif defined(SIR_MSEC_WIN32)
        static const ULONGLONG uepoch = 116444736e9;

        FILETIME ftutc = {0};  
        GetSystemTimePreciseAsFileTime(&ftutc);

        *tbuf = (((ULONGLONG)ftutc.dwHighDateTime << 32 | ftutc.dwLowDateTime & 0x0000ffff)
              - uepoch) / 1e7;

        if (nsecbuf) {
            SYSTEMTIME st = {0};
            FileTimeToSystemTime(&ftutc, &st);
            *nsecbuf = st.wMilliseconds;
        }
#else
        time(tbuf);
        if (nsecbuf)
            *nsecbuf = 0;
#endif
        return true;
    }

    return false;
}

pid_t _sir_getpid() {
#ifdef _WIN32
    return GetProcessId(GetCurrentProcess());
#elif __linux__
    return getpid();
#else
#pragma message "no support for process id on this platform."
#endif
}

pid_t _sir_gettid() {
#ifdef _WIN32
    return GetCurrentThreadId();
#elif __linux__
    return pthread_self();
#else
#pragma message "no support for thread id on this platform."
#endif
}

void _sir_handleerr_impl(sirerror_t err, const sirchar_t* func,
    const sirchar_t* file, uint32_t line) {
    if (SIR_NOERROR != err) {
#ifndef _WIN32
        errno = SIR_NOERROR;

        sirchar_t buf[SIR_MAXERROR] = {0};
        int finderr = strerror_r(err, buf, SIR_MAXERROR);

        _sir_selflog("%s: at %s:%d: error: (%d, '%s')\n", func, file,
            line, err, 0 == finderr ? buf : SIR_UNKERROR);    
#else
        DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS;
        TCHAR* errbuf = NULL;

        DWORD fmt = FormatMessage(flags, NULL, err, 0, (LPTSTR)&errbuf, SIR_MAXERROR, NULL);
        assert(fmt > 0);

        if (fmt > 0 && errbuf) {
            _sir_selflog("%s: at %s:%d: error: (%d, '%s')\n", func, file,
                line, err, errbuf);
            BOOL heapfree = HeapFree(GetProcessHeap(), 0, errbuf);
            assert(0 != heapfree);
        } else {
            _sir_selflog("%s: FormatMessage failed; err: %lu\n", __func__, GetLastError());
        }
#endif
    }
    assert(SIR_NOERROR == err);
}

void _sir_selflog(const sirchar_t* format, ...) {
#ifdef SIR_SELFLOG
    sirchar_t output[SIR_MAXMESSAGE] = {0};
    va_list   args;

    va_start(args, format);
    int print = vsnprintf(output, SIR_MAXMESSAGE, format, args);
    va_end(args);

    assert(print > 0);

    if (print > 0) {
        int put = fputs(output, stderr);
        assert(put != EOF);
    }
#else
    format = format;
#endif
}

#ifdef _WIN32
#ifdef DEBUG
void _sir_invalidparam(const wchar_t* expression, const wchar_t* function, const wchar_t* file,
    unsigned int line, uintptr_t pReserved) {
    const char* format =
        "%s: invalid paramter handler called:\n{\n\texpression: %S\n\tfunc: %S\n\tfile: %S\n\tline: %d";
#ifndef SIR_SELFLOG
    fprintf(stderr, format, __func__, expression, function, file, line);
#else
    _sir_selflog(format, __func__, expression, function, file, line);
#endif
    abort();
}
#endif
#endif

/*! \endcond PRIVATE */
