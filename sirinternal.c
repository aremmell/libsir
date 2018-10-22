/**
 * @file sirinternal.c
 * @brief Internal implementation.
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @copyright
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
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
#include "sirinternal.h"
#include "sirconsole.h"
#include "sirdefaults.h"
#include "sirfilecache.h"
#include "sirtextstyle.h"
#include "sirmutex.h"

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

bool _sir_sanity(void) {
    if (_SIR_MAGIC == _sir_magic)
        return true;
    _sir_seterror(_SIR_E_NOTREADY);
    return false;
}

bool _sir_options_sanity(const sirinit* si) {

    if (!_sir_validptr(si)) return false;

    bool levelcheck = true;
    levelcheck &= _sir_validlevels(si->d_stdout.levels);
    levelcheck &= _sir_validlevels(si->d_stderr.levels);

#ifndef SIR_NO_SYSLOG
    levelcheck &= _sir_validlevels(si->d_syslog.levels);
#endif

    bool optscheck = true;
    optscheck &= _sir_validopts(si->d_stdout.opts);
    optscheck &= _sir_validopts(si->d_stderr.opts);

    char* nullterm = strrchr(si->processName, '\0');
    return levelcheck && optscheck && _sir_validptr(nullterm);
}

bool _sir_init(sirinit* si) {

    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_validptr(si))
        return false;

    if (_sir_magic == _SIR_MAGIC) {
        _sir_seterror(_SIR_E_ALREADY);
        return false;
    }

    _sir_defaultlevels(&si->d_stdout.levels, sir_stdout_def_lvls);
    _sir_defaultopts(&si->d_stdout.opts, sir_stdout_def_opts);

    _sir_defaultlevels(&si->d_stderr.levels, sir_stderr_def_lvls);
    _sir_defaultopts(&si->d_stderr.opts, sir_stderr_def_opts);

#ifndef SIR_NO_SYSLOG
    _sir_defaultlevels(&si->d_syslog.levels, sir_syslog_def_lvls);
#endif

    if (!_sir_options_sanity(si))
        return false;

    sirinit* _si = _sir_locksection(_SIRM_INIT);
    assert(_si);

    if (_si) {
        memcpy(_si, si, sizeof(sirinit));

#ifndef SIR_NO_SYSLOG
        if (0 != _si->d_syslog.levels)
            openlog(_sir_validstrnofail(_si->processName) ? _si->processName : "",
                (_si->d_syslog.includePID ? LOG_PID : 0) | LOG_ODELAY, LOG_USER);
#endif

        _sir_magic = _SIR_MAGIC;
        _sir_unlocksection(_SIRM_INIT);
        return true;
    }

    return false;
}

void _sir_stdoutlevels(sirinit* si, sir_update_data* data) {
    si->d_stdout.levels = *data->levels;
}

void _sir_stdoutopts(sirinit* si, sir_update_data* data) {
    si->d_stdout.opts = *data->opts;
}

void _sir_stderrlevels(sirinit* si, sir_update_data* data) {
    si->d_stderr.levels = *data->levels;
}

void _sir_stderropts(sirinit* si, sir_update_data* data) {
    si->d_stderr.opts = *data->opts;
}

void _sir_sysloglevels(sirinit* si, sir_update_data* data) {
    si->d_syslog.levels = *data->levels;
}

bool _sir_writeinit(sir_update_data* data, sirinit_update update) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validupdatedata(data) && _sir_validptr(update)) {
        sirinit* si = _sir_locksection(_SIRM_INIT);
        assert(si);
        if (si) {
            update(si, data);
            return _sir_unlocksection(_SIRM_INIT);
        }
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

    if (!_sir_validptr(m))
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
            tmpsec = sir_override_styles;
            break;
        default: tmpm = NULL; tmpsec = NULL;
    }

    *m = tmpm;
    if (section)
        *section = tmpsec;

    return *m != NULL && (!section || *section != NULL);
}

bool _sir_cleanup(void) {

    if (!_sir_sanity())
        return false;

    bool cleanup = true;
    sirfcache* sfc     = _sir_locksection(_SIRM_FILECACHE);
    assert(sfc);

    if (cleanup &= NULL != sfc) {
        bool destroyfc = _sir_fcache_destroy(sfc);
        assert(destroyfc);
        cleanup &= _sir_unlocksection(_SIRM_FILECACHE) && destroyfc;
    }

    sirinit* si = _sir_locksection(_SIRM_INIT);
    assert(si);

    if (cleanup &= NULL != si) {
        memset(si, 0, sizeof(sirinit));
        cleanup &= _sir_unlocksection(_SIRM_INIT);
    }

    _sir_resettextstyles();
    _sir_magic = 0;
    _sir_selflog("%s: libsir is cleaned up\n", __func__);
    return cleanup;
}

#ifndef _WIN32
void _sir_initmutex_si_once(void) {
    _sir_initmutex(&si_mutex);
}

void _sir_initmutex_fc_once(void) {
    _sir_initmutex(&fc_mutex);
}

void _sir_initmutex_ts_once(void) {
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

    _sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validlevel(level) || !_sir_validstr(format))
        return false;

    sirinit* si = _sir_locksection(_SIRM_INIT);

    if (!si)
        return false;

    sirinit tmpsi = {0};
    memcpy(&tmpsi, si, sizeof(sirinit));
    _sir_unlocksection(_SIRM_INIT);

    sirbuf buf;
    siroutput output = {0};

    output.style = _sirbuf_get(&buf, _SIRBUF_STYLE);
    assert(output.style);

    bool appliedstyle = false;
    sir_textstyle style = _sir_gettextstyle(level);
    assert(SIRS_INVALID != style);

    if (SIRS_INVALID != style) {
        bool fmtstyle = _sir_formatstyle(style, output.style, SIR_MAXSTYLE);
        assert(fmtstyle);
        appliedstyle = fmtstyle;
    }

    if (!appliedstyle)
        _sir_resetstr(output.style);

    output.timestamp = _sirbuf_get(&buf, _SIRBUF_TIME);
    assert(output.timestamp);

    time_t now;
    long   nowmsec;
    bool   gettime = _sir_getlocaltime(&now, &nowmsec);
    assert(gettime);

    if (gettime) {
        bool fmttime = _sir_formattime(now, output.timestamp, SIR_TIMEFORMAT);
        assert(fmttime);

        if (!fmttime)
            _sir_resetstr(output.timestamp);

        output.msec = _sirbuf_get(&buf, _SIRBUF_MSEC);
        assert(output.msec);

        int fmtmsec = snprintf(output.msec, SIR_MAXMSEC, SIR_MSECFORMAT, nowmsec);
        assert(fmtmsec >= 0);

        if (fmtmsec < 0)
            _sir_resetstr(output.msec);
    } else {
        _sir_resetstr(output.msec);
    }

    output.level = _sirbuf_get(&buf, _SIRBUF_LEVEL);
    assert(output.level);
    snprintf(output.level, SIR_MAXLEVEL, SIR_LEVELFORMAT, _sir_levelstr(level));

    output.name = _sirbuf_get(&buf, _SIRBUF_NAME);
    assert(output.name);

    if (_sir_validstrnofail(tmpsi.processName)) {
        strncpy(output.name, tmpsi.processName, SIR_MAXNAME - 1);
    } else {
        _sir_resetstr(output.name);
    }

    output.pid = _sirbuf_get(&buf, _SIRBUF_PID);
    assert(output.pid);

    pid_t pid = _sir_getpid();
    int pidfmt = snprintf(output.pid, SIR_MAXPID, SIR_PIDFORMAT, pid);
    assert(pidfmt >= 0);

    if (pidfmt < 0)
        _sir_resetstr(output.pid);

    pid_t tid  = _sir_gettid();
    output.tid = _sirbuf_get(&buf, _SIRBUF_TID);
    assert(output.tid);

    if (tid != pid) {
       if (!_sir_getthreadname(output.tid)) {
           pidfmt = snprintf(output.tid, SIR_MAXPID, SIR_PIDFORMAT, tid);
           assert(pidfmt >= 0);

           if (pidfmt < 0)
            _sir_resetstr(output.tid);
       }
    } else {
        _sir_resetstr(output.tid);
    }

    /** @todo add support for glibc's %%m? */
    output.message = _sirbuf_get(&buf, _SIRBUF_MSG);
    assert(output.message);
    int msgfmt = vsnprintf(output.message, SIR_MAXMESSAGE, format, args);
    assert(msgfmt >= 0);

    if (msgfmt < 0)
        _sir_resetstr(output.message);

    output.output = _sirbuf_get(&buf, _SIRBUF_OUTPUT);
    assert(output.output);

    return _sir_dispatch(&tmpsi, level, &output);
}

bool _sir_dispatch(sirinit* si, sir_level level, siroutput* output) {

    if (_sir_validptr(output)) {
        bool   r          = true;
        size_t dispatched = 0;
        size_t wanted = 0;

        if (_sir_bittest(si->d_stdout.levels, level)) {
            const sirchar_t* write = write = _sir_format(true, si->d_stdout.opts, output);
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
            wanted++;
        }

        if (_sir_bittest(si->d_stderr.levels, level)) {
            const sirchar_t* write = write = _sir_format(true, si->d_stderr.opts, output);
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
            wanted++;
        }

#ifndef SIR_NO_SYSLOG
        if (_sir_bittest(si->d_syslog.levels, level)) {
            syslog(_sir_syslog_maplevel(level), "%s", output->message);
            dispatched++;
            wanted++;
        }
#endif
        sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);

        if (sfc) {
            size_t fdispatched = 0;
            size_t fwanted = 0;
            r &= _sir_fcache_dispatch(sfc, level, output, &fdispatched, &fwanted);
            r &= _sir_unlocksection(_SIRM_FILECACHE);
            dispatched += fdispatched;
            wanted += fwanted;
        }

        if (0 == wanted) {
            _sir_seterror(_SIR_E_NODEST);
            return false;
        }

        return r && (dispatched == wanted);
    }

    return false;
}

const sirchar_t* _sir_format(bool styling, sir_options opts, siroutput* output) {

    if (_sir_validopts(opts) && _sir_validptr(output) && _sir_validptr(output->output)) {
        bool first = true;

        _sir_resetstr(output->output);

#ifndef _WIN32
        if (styling)
            strncat(output->output, output->style, SIR_MAXSTYLE);
#endif

        if (!_sir_bittest(opts, SIRO_NOTIME)) {
            strncat(output->output, output->timestamp, SIR_MAXTIME);
            first = false;

#ifdef SIR_MSEC_TIMER
            if (!_sir_bittest(opts, SIRO_NOMSEC))
                strncat(output->output, output->msec, SIR_MAXMSEC);
#endif
        }

        if (!_sir_bittest(opts, SIRO_NOLEVEL)) {
            if (!first)
                strncat(output->output, " ", 1);
            strncat(output->output, output->level, SIR_MAXLEVEL);
            first = false;
        }

        bool name = false;
        if (!_sir_bittest(opts, SIRO_NONAME) && _sir_validstrnofail(output->name)) {
            if (!first)
                strncat(output->output, " ", 1);
            strncat(output->output, output->name, SIR_MAXNAME);
            first = false;
            name  = true;
        }

        bool wantpid = !_sir_bittest(opts, SIRO_NOPID) && _sir_validstrnofail(output->pid);
        bool wanttid = !_sir_bittest(opts, SIRO_NOTID) && _sir_validstrnofail(output->tid);

        if (wantpid || wanttid) {
            if (name)
                strncat(output->output, "(", 1);
            else if (!first)
                strncat(output->output, " ", 1);

            if (wantpid)
                strncat(output->output, output->pid, SIR_MAXPID);

            if (wanttid) {
                if (wantpid)
                    strncat(output->output, SIR_PIDSEPARATOR, 1);
                strncat(output->output, output->tid, SIR_MAXPID);
            }

            if (name)
                strncat(output->output, ")", 1);
        }

        if (!first)
            strncat(output->output, ": ", 2);

        strncat(output->output, output->message, SIR_MAXMESSAGE);

#ifndef _WIN32
        if (styling)
            strncat(output->output, SIR_ENDSTYLE, SIR_MAXSTYLE);
#endif

        strncat(output->output, "\n", 1);
        return output->output;
    }

    return NULL;
}

#ifndef SIR_NO_SYSLOG
int _sir_syslog_maplevel(sir_level level) {

    assert(_sir_validlevel(level));

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

/* In case there's a better way to implement this, abstract it away. */
sirchar_t* _sirbuf_get(sirbuf* buf, size_t idx) {

    assert(idx <= _SIRBUF_MAX);

    switch (idx) {
        case _SIRBUF_STYLE: return buf->style;
        case _SIRBUF_TIME: return buf->timestamp;
        case _SIRBUF_MSEC: return buf->msec;
        case _SIRBUF_LEVEL: return buf->level;
        case _SIRBUF_NAME: return buf->name;
        case _SIRBUF_PID: return buf->pid;
        case _SIRBUF_TID: return buf->tid;
        case _SIRBUF_MSG: return buf->message;
        case _SIRBUF_OUTPUT: return buf->output;
        default: assert(false);
    }

    return NULL;
}

const sirchar_t* _sir_levelstr(sir_level level) {

    assert(_sir_validlevel(level));

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

bool _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format) {
    if (0 != now && _sir_validptr(buffer) && _sir_validstr(format)) {
        size_t fmttime = strftime(buffer, SIR_MAXTIME, format, localtime(&now));
        assert(0 != fmttime);

        if (0 == fmttime)
            _sir_selflog("%s: strftime returned 0; format string: '%s'", __func__, format);

        return 0 != fmttime;
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

        ULARGE_INTEGER ftnow;
        ftnow.HighPart = ftutc.dwHighDateTime;
        ftnow.LowPart  = ftutc.dwLowDateTime;
        ftnow.QuadPart = (ftnow.QuadPart - uepoch) / 1e7;

        *tbuf = (time_t)ftnow.QuadPart;

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

pid_t _sir_getpid(void) {
#ifndef _WIN32
    return getpid();
#else
    return (pid_t)GetProcessId(GetCurrentProcess());
#endif
}

pid_t _sir_gettid(void) {
    pid_t tid = 0;
#ifdef __MACOS__
    uint64_t tid64 = 0;
    int gettid = pthread_threadid_np(NULL, &tid64);
    if (0 != gettid) _sir_handleerr(gettid);
    tid = (pid_t)tid64;
#elif defined(_WIN32)
    tid = (pid_t)GetCurrentThreadId();
#else
    tid = syscall(SYS_gettid);
#endif
    return tid;
}

bool _sir_getthreadname(char name[SIR_MAXPID]) {
#ifdef _GNU_SOURCE
    return 0 == pthread_getname_np(pthread_self(), name, SIR_MAXPID);
#else
    return false;
#endif
}

/** @} */
