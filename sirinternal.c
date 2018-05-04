/**
 * @file sirinternal.c
 * @brief Internal implementation.
 */
#include "sirinternal.h"
#include "sirconsole.h"
#include "sirdefaults.h"
#include "sirfilecache.h"
#include "sirmutex.h"
#include "sirtextstyle.h"

/**
 * @addtogroup intern
 * @{
 */

static sirinit   _sir_si = {0};
static sirfcache _sir_fc = {0};

static sirmutex_t si_mutex;
static sironce_t  si_once = SIR_ONCE_INIT;

static sirmutex_t fc_mutex;
static sironce_t  fc_once = SIR_ONCE_INIT;

static sirmutex_t ts_mutex;
static sironce_t  ts_once = SIR_ONCE_INIT;

static volatile uint32_t _sir_magic;

bool _sir_sanity() {
    if (_SIR_MAGIC == _sir_magic)
        return true;

    _sir_selflog("%s: sanity check failed; has sir_init been called?\n", __func__);
    return false;
}

bool _sir_options_sanity(const sirinit* si) {
#pragma message "_sir_options_sanity"
    return true;
}

bool _sir_init(const sirinit* si) {
    if (_sir_sanity()) {
        _sir_selflog("%s: sir appears to already be initialized!\n", __func__);
        return false;
    }

    if (!si || !_sir_options_sanity(si))
        return false;

    sirinit* _si = _sir_locksection(_SIRM_INIT);
    assert(_si);

    if (_si) {
        memcpy(_si, si, sizeof(sirinit));

#ifndef SIR_NO_SYSLOG
        // TODO: if not using process name, use pid for syslog identity?
        if (0 != _si->d_syslog.levels)
            openlog(validstr(_si->processName) ? _si->processName : "",
                (_si->d_syslog.includePID ? LOG_PID : 0) | LOG_ODELAY, LOG_USER);
#endif
        _sir_magic = _SIR_MAGIC;

        _sir_unlocksection(_SIRM_INIT);
        _sir_selflog("%s: SIR is initialized\n", __func__);
        return true;
    }

    return false;
}

void* _sir_locksection(sir_mutex_id mid) {

    sirmutex_t* m   = NULL;
    void*       sec = NULL;

    if (_sir_mapmutexid(mid, &m, &sec)) {
        bool enter = _sirmutex_lock(m);
        assert(enter);

        return enter ? sec : NULL;
    }

    return NULL;
}

bool _sir_unlocksection(sir_mutex_id mid) {

    sirmutex_t* m   = NULL;
    void*       sec = NULL;

    if (_sir_mapmutexid(mid, &m, &sec)) {
        bool leave = _sirmutex_unlock(m);
        assert(leave);

        return leave;
    }

    return false;
}

bool _sir_mapmutexid(sir_mutex_id mid, sirmutex_t** m, void** section) {

    assert(m);

    if (!m)
        return false;

    sirmutex_t* tmpm;
    void*       tmpsec;

    switch (mid) {
        case _SIRM_INIT:
            _sir_once(&si_once, _sir_initmutex_si_once);
            tmpm   = &si_mutex;
            tmpsec = &_sir_si;
            break;
        case _SIRM_FILECACHE:
            _sir_once(&fc_once, _sir_initmutex_fc_once);
            tmpm   = &fc_mutex;
            tmpsec = &_sir_fc;
            break;
        case _SIRM_TEXTSTYLE:
            _sir_once(&ts_once, _sir_initmutex_ts_once);
            tmpm   = &ts_mutex;
            tmpsec = sir_default_styles;
            break;
        default: tmpm = NULL; tmpsec = NULL;
    }

    *m = tmpm;
    if (section)
        *section = tmpsec;

    return *m != NULL && (!section || *section != NULL);
}

bool _sir_cleanup() {

    if (!_sir_sanity())
        return false;

    bool       cleanup = true;
    sirfcache* sfc     = _sir_locksection(_SIRM_FILECACHE);
    assert(sfc);
    cleanup &= NULL != sfc;

    if (sfc) {
        bool destroyfc = _sir_fcache_destroy(sfc);
        assert(destroyfc);
        cleanup &= _sir_unlocksection(_SIRM_FILECACHE) && destroyfc;
    }

    sirinit* si = _sir_locksection(_SIRM_INIT);
    assert(si);
    cleanup &= NULL != si;

    if (si) {
        memset(si, 0, sizeof(sirinit));
        cleanup &= _sir_unlocksection(_SIRM_INIT);
    }

    _sir_magic = 0;
    _sir_selflog("%s: SIR is cleaned up\n", __func__);
    return cleanup;
}

#ifndef _WIN32
void _sir_initmutex_si_once() {
    _sir_initmutex(&si_mutex);
}

void _sir_initmutex_fc_once() {
    _sir_initmutex(&fc_mutex);
}

void _sir_initmutex_ts_once() {
    _sir_initmutex(&ts_mutex);
}
#else
BOOL CALLBACK _sir_initmutex_si_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _sir_initmutex(&si_mutex);
    return TRUE;
}

BOOL CALLBACK _sir_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _sir_initmutex(&fc_mutex);
    return TRUE;
}

BOOL CALLBACK _sir_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _sir_initmutex(&ts_mutex);
    return TRUE;
}
#endif

void _sir_initmutex(sirmutex_t* mutex) {
    bool init = _sirmutex_create(mutex);
    assert(init);
}

void _sir_once(sironce_t* once, sir_once_fn func) {
#ifndef _WIN32
    pthread_once(once, func);
#else
    BOOL result = InitOnceExecuteOnce(once, func, NULL, NULL);
    assert(FALSE != result);
#endif
}

bool _sir_logv(sir_level level, const sirchar_t* format, va_list args) {

    if (!_sir_sanity())
        return false;

    assert(validlevel(level));
    assert(validstr(format));

    if (!validlevel(level) || !validstr(format))
        return false;

    sirinit* si = _sir_locksection(_SIRM_INIT);

    if (!si)
        return false;

    sirinit tmpsi = {0};
    memcpy(&tmpsi, si, sizeof(sirinit));
    _sir_unlocksection(_SIRM_INIT);

    sirbuf    buf;
    siroutput output = {0};

    output.style = _sirbuf_get(&buf, _SIRBUF_STYLE);
    assert(output.style);

    sir_textstyle defstyle = _sir_getdefstyle(level);
    assert(SIRS_INVALID != defstyle);

    bool fmtstyle = _sir_formatstyle(defstyle, output.style, SIR_MAXSTYLE);
    assert(fmtstyle);

    output.timestamp = _sirbuf_get(&buf, _SIRBUF_TIME);
    assert(output.timestamp);

    time_t now;
    long   nowmsec;
    bool   gettime = _sir_getlocaltime(&now, &nowmsec);
    assert(gettime);

    if (gettime) {
        bool fmt = _sir_formattime(now, output.timestamp, SIR_TIMEFORMAT);
        assert(fmt);

        output.msec = _sirbuf_get(&buf, _SIRBUF_MSEC);
        assert(output.msec);

        snprintf(output.msec, SIR_MAXMSEC, SIR_MSECFORMAT, nowmsec);
    }

    output.level = _sirbuf_get(&buf, _SIRBUF_LEVEL);
    assert(output.level);
    snprintf(output.level, SIR_MAXLEVEL, "[%s]", _sir_levelstr(level));

    if (validstr(tmpsi.processName)) {
        output.name = _sirbuf_get(&buf, _SIRBUF_NAME);
        assert(output.name);
        strncpy(output.name, tmpsi.processName, SIR_MAXNAME - 1);
#pragma message "TODO: PID/TID"
        // snprintf(output.name, SIR_MAXNAME, "%s (%d:%d)", sir_s.processName, _sir_getpid(), _sir_gettid());
    }

    /** \todo add support for syslog's %m */
    output.message = _sirbuf_get(&buf, _SIRBUF_MSG);
    assert(output.message);
    int msgfmt = vsnprintf(output.message, SIR_MAXMESSAGE, format, args);
    assert(msgfmt >= 0);

    if (msgfmt < 0)
        _sir_selflog("%s: vsnprintf returned %d!", __func__, msgfmt);

    output.output = _sirbuf_get(&buf, _SIRBUF_OUTPUT);
    assert(output.output);

    return _sir_dispatch(&tmpsi, level, &output);
}

bool _sir_dispatch(sirinit* si, sir_level level, siroutput* output) {

    bool   r          = true;
    size_t dispatched = 0;

    assert(validlevel(level));
    assert(output);

    if (validlevel(level) && output) {
        if (_sir_destwantslevel(si->d_stderr.levels, level)) {
            const sirchar_t* write = _sir_format(true, si->d_stderr.opts, output);
            assert(write);
#ifndef _WIN32
            bool wrote = _sir_stderr_write(write);
            r &= NULL != write && wrote;
#else
            uint16_t* style = (uint16_t*)output->style;
            bool      wrote = _sir_stderr_write(*style, write);
            r &= NULL != write && NULL != style && wrote;
#endif
            if (wrote)
                dispatched++;
        }

        if (_sir_destwantslevel(si->d_stdout.levels, level)) {
            const sirchar_t* write = _sir_format(true, si->d_stdout.opts, output);
            assert(write);
#ifndef _WIN32
            bool wrote = _sir_stdout_write(write);
            r &= NULL != write && wrote;
#else
            uint16_t* style = (uint16_t*)output->style;
            bool      wrote = _sir_stdout_write(*style, write);
            r &= NULL != write && NULL != style && wrote;
#endif
            if (wrote)
                dispatched++;
        }

#ifndef SIR_NO_SYSLOG
        if (_sir_destwantslevel(si->d_syslog.levels, level)) {
            syslog(_sir_syslog_maplevel(level), "%s", output->message);
            dispatched++;
        }
#endif
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);

        if (sfc) {
            size_t fdispatched = 0;
            r &= _sir_fcache_dispatch(sfc, level, output, &fdispatched);
            r &= _sir_unlocksection(_SIRM_FILECACHE);
            dispatched += fdispatched;
        }

        return r && (dispatched > 0);
    }

    return false;
}

const sirchar_t* _sir_format(bool styling, sir_options opts, siroutput* output) {

    assert(validopts(opts));
    assert(output);
    assert(output->output);

    if (validopts(opts) && output && output->output) {
        bool first = true;

        resetstr(output->output);

        if (styling) {
#ifndef _WIN32
            strncat(output->output, output->style, SIR_MAXSTYLE);
#endif
        }

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

        if (!flagtest(opts, SIRO_NONAME) && validstr(output->name)) {
            if (!first)
                strncat(output->output, " ", 1);
            strncat(output->output, output->name, SIR_MAXNAME);
            first = false;
        }

        if (!first)
            strncat(output->output, ": ", 2);

        strncat(output->output, output->message, SIR_MAXMESSAGE);

        if (styling) {
#ifndef _WIN32
            strncat(output->output, SIR_ENDSTYLE, SIR_MAXSTYLE);
#endif
        }

        strncat(output->output, "\n", 1);
        return output->output;
    }

    return NULL;
}

#ifndef SIR_NO_SYSLOG
int _sir_syslog_maplevel(sir_level level) {

    assert(validlevel(level));

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

/* in case there's a better way to implement this
 * hms, abstract it away.
 */
sirchar_t* _sirbuf_get(sirbuf* buf, size_t idx) {

    assert(idx <= _SIRBUF_MAX);

    switch (idx) {
        case _SIRBUF_STYLE: return buf->style;
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

const sirchar_t* _sir_levelstr(sir_level level) {

    assert(validlevel(level));

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
        time(tbuf);
#ifdef SIR_MSEC_POSIX
        struct timespec ts    = {0};
        int             clock = clock_gettime(SIR_MSECCLOCK, &ts);
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

        *tbuf = (((ULONGLONG)ftutc.dwHighDateTime << 32 | ftutc.dwLowDateTime & 0x0000ffff) - uepoch) / 1e7;

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
#ifndef _WIN32
    return getpid();
#else
    return (pid_t)GetProcessId(GetCurrentProcess());
#endif
}

pid_t _sir_gettid() {
#ifndef _WIN32
    return syscall(SYS_gettid);
#else
    return (pid_t)GetCurrentThreadId();
#endif
}

#ifdef SIR_SELFLOG
void _sir_handleerr_impl(sirerror_t err, const sirchar_t* func, const sirchar_t* file, uint32_t line) {
    if (SIR_NOERROR != err) {
#ifndef _WIN32
        errno = SIR_NOERROR;

        sirchar_t buf[SIR_MAXERROR] = {0};
        int       finderr           = strerror_r(err, buf, SIR_MAXERROR);

        _sir_selflog(
            "%s: at %s:%d: error: (%d, '%s')\n", func, file, line, err, 0 == finderr ? buf : SIR_UNKERROR);
#else
        DWORD flags =
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        TCHAR* errbuf = NULL;

        DWORD fmt = FormatMessage(flags, NULL, err, 0, (LPTSTR)&errbuf, SIR_MAXERROR, NULL);
        assert(fmt > 0);

        if (fmt > 0 && errbuf) {
            _sir_selflog("%s: at %s:%d: error: (%d, '%s')\n", func, file, line, err, errbuf);
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
}
#endif

/** @} */
