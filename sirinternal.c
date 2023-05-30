/**
 * @file sirinternal.c
 * @brief Internal implementation.
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/libsir>.
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
#include "sirfilesystem.h"
#include "sirmutex.h"

/**
 * @addtogroup intern
 * @{
 */

static sirinit _sir_si   = {0};
static sirfcache _sir_fc = {0};

static sirmutex_t si_mutex;
static sironce_t si_once = SIR_ONCE_INIT;

static sirmutex_t fc_mutex;
static sironce_t fc_once = SIR_ONCE_INIT;

static sirmutex_t ts_mutex;
static sironce_t ts_once = SIR_ONCE_INIT;

#if !defined(_WIN32)
static atomic_uint_fast32_t _sir_magic;
#else
static volatile uint32_t _sir_magic;
#endif

static sironce_t magic_once = SIR_ONCE_INIT;

bool _sir_sanity(void) {
#if !defined(_WIN32)
    if (_SIR_MAGIC == atomic_load(&_sir_magic))
        return true;
#else
    if (_SIR_MAGIC == _sir_magic)
        return true;
#endif

    _sir_seterror(_SIR_E_NOTREADY);
    return false;
}

bool _sir_options_sanity(const sirinit* si) {
    if (!_sir_validptr(si))
        return false;

    bool levelcheck = true;
    levelcheck &= _sir_validlevels(si->d_stdout.levels);
    levelcheck &= _sir_validlevels(si->d_stderr.levels);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    levelcheck &= _sir_validlevels(si->d_syslog.levels);
#endif

    bool optscheck = true;
    optscheck &= _sir_validopts(si->d_stdout.opts);
    optscheck &= _sir_validopts(si->d_stderr.opts);

    return levelcheck && optscheck;
}

bool _sir_init(sirinit* si) {

    _sir_seterror(_SIR_E_NOERROR);
    _sir_once(&magic_once, _sir_initialize_once);

    if (!_sir_validptr(si))
        return false;

#if !defined(_WIN32)
    if (_SIR_MAGIC == atomic_load(&_sir_magic)) {
#else
    if (_SIR_MAGIC == _sir_magic) {
#endif
        _sir_seterror(_SIR_E_ALREADY);
        return false;
    }

    _sir_defaultlevels(&si->d_stdout.levels, sir_stdout_def_lvls);
    _sir_defaultopts(&si->d_stdout.opts, sir_stdout_def_opts);

    _sir_defaultlevels(&si->d_stderr.levels, sir_stderr_def_lvls);
    _sir_defaultopts(&si->d_stderr.opts, sir_stderr_def_opts);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_defaultlevels(&si->d_syslog.levels, sir_syslog_def_lvls);
#endif

    if (!_sir_options_sanity(si))
        return false;

#if defined(_WIN32)
    if (!_sir_initialize_stdio())
        return false;
#endif

    sirinit* _si = _sir_locksection(_SIRM_INIT);
    assert(_si);

    if (_sir_validptr(_si)) {
        memcpy(_si, si, sizeof(sirinit));

#if !defined(_WIN32)
        atomic_store(&_sir_magic, _SIR_MAGIC);
#else
        _sir_magic = _SIR_MAGIC;
#endif

        /* forcibly null-terminate the process name. */
        _si->processName[SIR_MAXNAME - 1] = '\0';

        /* initialize syslog/os_log */
        _sir_syslog_open(_si->processName, &_si->d_syslog);

        bool unlock = _sir_unlocksection(_SIRM_INIT);
        assert(unlock);
        return true;
    }

    return false;
}

static void _sir_updatelevels(const char* name, sir_levels* old, sir_levels* new) {
    if (*old != *new) {
        _sir_selflog("%s: updating %s levels from %04x to %04x\n", __func__, name, *old, *new);
        *old = *new;
        return;
    }

    _sir_selflog("%s: superfluous update of %s levels: %04x\n", __func__, name, *old);
}

static void _sir_updateopts(const char* name, sir_options* old, sir_options* new) {
    if (*old != *new) {
        _sir_selflog("%s: updating %s options from %08x to %08x\n", __func__, name, *old, *new);
        *old = *new;
        return;
    }

    _sir_selflog("%s: superfluous update of %s options: %08x\n", __func__, name, *old);
}

void _sir_stdoutlevels(sirinit* si, sir_update_config_data* data) {
    _sir_updatelevels("stdout", &si->d_stdout.levels, data->levels);
}

void _sir_stdoutopts(sirinit* si, sir_update_config_data* data) {
    _sir_updateopts("stdout", &si->d_stdout.opts, data->opts);
}

void _sir_stderrlevels(sirinit* si, sir_update_config_data* data) {
    _sir_updatelevels("stderr", &si->d_stderr.levels, data->levels);
}

void _sir_stderropts(sirinit* si, sir_update_config_data* data) {
    _sir_updateopts("stderr", &si->d_stderr.opts, data->opts);
}

void _sir_sysloglevels(sirinit* si, sir_update_config_data* data) {
    _sir_updatelevels("syslog", &si->d_syslog.levels, data->levels);
}

void _sir_syslogid(sirinit* si, sir_update_config_data* data) {
    int update = _sir_strncpy(si->d_syslog.identity, SIR_MAX_SYSLOG_ID,
        data->sl_identity, strnlen(data->sl_identity, SIR_MAX_SYSLOG_ID));
    _sir_selflog("%s: updated syslog identity to: '%s'", __func__, data->sl_identity);
}

void _sir_syslogcat(sirinit* si, sir_update_config_data* data) {
    int update = _sir_strncpy(si->d_syslog.category, SIR_MAX_SYSLOG_ID,
        data->sl_category, strnlen(data->sl_category, SIR_MAX_SYSLOG_ID));
    _sir_selflog("%s: updated syslog category to: '%s'", __func__, data->sl_category);
}

bool _sir_writeinit(sir_update_config_data* data, sirinit_update update) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validupdatedata(data) && _sir_notnull(update)) {
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

    sirmutex_t* m = NULL;
    void* sec     = NULL;

    if (_sir_mapmutexid(mid, &m, &sec)) {
        bool enter = _sirmutex_lock(m);
        assert(enter);
        return enter ? sec : NULL;
    }

    return NULL;
}

bool _sir_unlocksection(sir_mutex_id mid) {

    sirmutex_t* m = NULL;
    void* sec     = NULL;

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
    void* tmpsec;

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
            tmpsec = sir_override_style_map;
            break;
        default:
            assert("!invalid mutex id");
            tmpm = NULL; tmpsec = NULL;
        break;
    }

    *m = tmpm;
    if (section)
        *section = tmpsec;

    return *m != NULL && (!section || *section != NULL);
}

bool _sir_cleanup(void) {

    if (!_sir_sanity())
        return false;

    bool cleanup   = true;
    sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);
    assert(sfc);

    if (_sir_notnull(sfc)) {
        bool destroyfc = _sir_fcache_destroy(sfc);
        assert(destroyfc);
        cleanup &= _sir_unlocksection(_SIRM_FILECACHE) && destroyfc;
    }

    sirinit* si = _sir_locksection(_SIRM_INIT);
    assert(si);
    cleanup &= _sir_notnull(si);

    if (_sir_notnull(si)) {
        _sir_syslog_close(&si->d_syslog);
        _sir_resettextstyles();

#if !defined(_WIN32)
    atomic_store(&_sir_magic, 0);
#else
    _sir_magic = 0;
#endif
        memset(si, 0, sizeof(sirinit));
        cleanup &= _sir_unlocksection(_SIRM_INIT);
    }

    _sir_selflog("%s: cleanup: %s\n", __func__, (cleanup ? "successful" : "with issues"));

    assert(cleanup);
    return cleanup;
}

#if !defined(_WIN32)
void _sir_initialize_once(void) {
    atomic_init(&_sir_magic, 0);
}

void _sir_initmutex_si_once(void) {
    _sirmutex_create(&si_mutex);
}

void _sir_initmutex_fc_once(void) {
    _sirmutex_create(&fc_mutex);
}

void _sir_initmutex_ts_once(void) {
    _sirmutex_create(&ts_mutex);
#pragma message("TODO: figure out what you're supposed to do to handle an error in a phtread_once routine")    
}
#else
BOOL CALLBACK _sir_initialize_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _SIR_UNUSED(ponce);
    _SIR_UNUSED(param);
    _SIR_UNUSED(ctx)
    // atomic_init(&_sir_magic, 0);
    return TRUE;
}

BOOL CALLBACK _sir_initmutex_si_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _SIR_UNUSED(ponce);
    _SIR_UNUSED(param);
    _SIR_UNUSED(ctx)
    return _sir_initmutex(&si_mutex) ? TRUE : FALSE;
}

BOOL CALLBACK _sir_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _SIR_UNUSED(ponce);
    _SIR_UNUSED(param);
    _SIR_UNUSED(ctx)
    return _sir_initmutex(&fc_mutex) ? TRUE : FALSE
}

BOOL CALLBACK _sir_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    _SIR_UNUSED(ponce);
    _SIR_UNUSED(param);
    _SIR_UNUSED(ctx)
    return _sir_initmutex(&ts_mutex) ? TRUE : FALSE
}
#endif

bool _sir_once(sironce_t *once, sir_once_fn func) {
#if !defined(_WIN32)
    int ret = pthread_once(once, func);
    if (0 != ret) {
        _sir_handleerr(ret);
        return false;
    }
    return true;
#else
    BOOL ret = InitOnceExecuteOnce(once, func, NULL, NULL);
    if (!ret) {
        _sir_handlewin32err(GetLastError());
        return false;
    }
    return true;
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

    sirbuf buf = {0};
    
    bool appliedstyle = false;
    sir_textstyle style = _sir_gettextstyle(level);
    assert(SIRS_INVALID != style);

    if (SIRS_INVALID != style) {
        appliedstyle = _sir_formatstyle(style, buf.style, SIR_MAXSTYLE);
        assert(appliedstyle);
    }

    time_t now;
    long nowmsec;
    bool gettime = _sir_getlocaltime(&now, &nowmsec);
    assert(gettime);

    if (gettime) {
        bool fmttime = _sir_formattime(now, buf.timestamp, SIR_TIMEFORMAT);
        _SIR_UNUSED(fmttime);

        if (0 > snprintf(buf.msec, SIR_MAXMSEC, SIR_MSECFORMAT, nowmsec))
            _sir_handleerr(errno);
    }

    if (0 > snprintf(buf.level, SIR_MAXLEVEL, SIR_LEVELFORMAT, _sir_levelstr(level)))
        _sir_handleerr(errno);

    if (_sir_validstrnofail(tmpsi.processName))
        _sir_strncpy(buf.name, SIR_MAXNAME, tmpsi.processName, SIR_MAXNAME);

    pid_t pid = _sir_getpid();
    if (0 > snprintf(buf.pid, SIR_MAXPID, SIR_PIDFORMAT, pid))
        _sir_handleerr(errno);

    pid_t tid = _sir_gettid();
    if (tid != pid) {
        if (!_sir_getthreadname(buf.tid)) {
            if (0 > snprintf(buf.tid, SIR_MAXPID, SIR_PIDFORMAT, tid))
                _sir_handleerr(errno);
        }
    }

    if (0 > vsnprintf(buf.message, SIR_MAXMESSAGE, format, args))
        _sir_handleerr(errno);

    return _sir_dispatch(&tmpsi, level, &buf);
}

bool _sir_dispatch(sirinit* si, sir_level level, sirbuf* buf) {

    bool retval       = true;
    size_t dispatched = 0;
    size_t wanted     = 0;

    if (_sir_bittest(si->d_stdout.levels, level)) {
        const sirchar_t* write = _sir_format(true, si->d_stdout.opts, buf);
        bool wrote = _sir_validstrnofail(write) &&
            _sir_write_stdout(write, buf->output_len);
        retval &= wrote;

        if (wrote)
            dispatched++;
        wanted++;
    }

    if (_sir_bittest(si->d_stderr.levels, level)) {
        const sirchar_t* write = _sir_format(true, si->d_stderr.opts, buf);
        bool wrote = _sir_validstrnofail(write) &&
            _sir_write_stderr(write, buf->output_len);
        retval &= wrote;

        if (wrote)
            dispatched++;
        wanted++;
    }

    if (_sir_bittest(si->d_syslog.levels, level)) {
        if (_sir_syslog_write(level, buf, &si->d_syslog))
            dispatched++;
        wanted++;
    }

    sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);
    if (sfc) {
        size_t fdispatched = 0;
        size_t fwanted = 0;
        retval &= _sir_fcache_dispatch(sfc, level, buf, &fdispatched, &fwanted);
        retval &= _sir_unlocksection(_SIRM_FILECACHE);
        dispatched += fdispatched;
        wanted += fwanted;
    }

    if (0 == wanted) {
        _sir_seterror(_SIR_E_NODEST);
        _sir_selflog("%s: no destinations registered to for level %04x\n", __func__, level);
        return false;
    }

    return retval && (dispatched == wanted);
}

const sirchar_t* _sir_format(bool styling, sir_options opts, sirbuf* buf) {

    if (_sir_validopts(opts) && _sir_validptr(buf) && _sir_validptr(buf->output)) {
        bool first = true;

        _sir_resetstr(buf->output);

        if (styling)
            _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->style, SIR_MAXSTYLE);

        if (!_sir_bittest(opts, SIRO_NOTIME)) {
            _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->timestamp, SIR_MAXTIME);
            first = false;

#if defined(SIR_MSEC_TIMER)
            if (!_sir_bittest(opts, SIRO_NOMSEC))
                _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->msec, SIR_MAXMSEC);
#endif
        }

        if (!_sir_bittest(opts, SIRO_NOLEVEL)) {
            if (!first)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);
            _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->level, SIR_MAXLEVEL);
            first = false;
        }

        bool name = false;
        if (!_sir_bittest(opts, SIRO_NONAME) && _sir_validstrnofail(buf->name)) {
            if (!first)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);
            _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->name, SIR_MAXNAME);
            first = false;
            name  = true;
        }

        bool wantpid = !_sir_bittest(opts, SIRO_NOPID) && _sir_validstrnofail(buf->pid);
        bool wanttid = !_sir_bittest(opts, SIRO_NOTID) && _sir_validstrnofail(buf->tid);

        if (wantpid || wanttid) {
            if (name)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, "(", 1);
            else if (!first)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);

            if (wantpid)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->pid, SIR_MAXPID);

            if (wanttid) {
                if (wantpid)
                    _sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_PIDSEPARATOR, 1);
                _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->tid, SIR_MAXPID);
            }

            if (name)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, ")", 1);
        }

        if (!first)
            _sir_strncat(buf->output, SIR_MAXOUTPUT, ": ", 2);

        _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->message, SIR_MAXMESSAGE);

        if (styling)
            _sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_ENDSTYLE, SIR_MAXSTYLE);

        _sir_strncat(buf->output, SIR_MAXOUTPUT, "\n", 1);

        buf->output_len = strnlen(buf->output, SIR_MAXOUTPUT);

        return buf->output;
    }

    return NULL;
}

#if !defined(SIR_NO_SYSTEM_LOGGERS)

/*bool _sir_syslog_init(const char *name, sir_syslog_dest *ctx) {
    char identity[SIR_MAXNAME] = {0};
    if (_sir_validstrnofail(name)) {
        /* using name from global config. * /
        if (SIRSL_SYSLOG == sir_syslog_type) {
            _sir_strncpy(identity, SIR_MAXNAME, name, strnlen(name, SIR_MAXNAME));
        } else {
            snprintf(identity, SIR_MAXNAME, "com.%s.%s", name, SIR_LIBNAME);
        }
    } else {
        /* no name in config; using name of binary * /
        char appfilename[SIR_MAXPATH] = {0};
        if (_sir_getappfilename(appfilename, SIR_MAXPATH)) {
            if (SIRSL_OS_LOG == sir_syslog_type) {
                _sir_strncpy(identity, SIR_MAXNAME, appfilename, strnlen(appfilename, SIR_MAXNAME));
            } else {
                snprintf(identity, SIR_MAXNAME, "com.%s.%s", appfilename, SIR_LIBNAME);
            }
        } else {
            /* everything failed; using 'SIR_LIBNAME' * /
            _sir_strncpy(identity, SIR_MAXNAME, SIR_LIBNAME, strnlen(SIR_LIBNAME, SIR_MAXNAME));
        }
    }
}*/

bool _sir_syslog_open(const char *name, sir_syslog_dest *ctx) {
    if (ctx->opened_log) {
        _sir_selflog("%s: already opened log; ignoring\n", __func__);
        return false;
    }

#if defined(SIR_OS_LOG_ENABLED)
#pragma message("TODO: finish implementation of init, take category/identity from sirinit")
const char *identity = "foo";
   const char *category = "minutiae";

    ctx->logger = os_log_create(identity, category);
    ctx->opened_log = true;
    _sir_selflog("%s: opened os_log ('%s', '%s')\n", __func__, identity, category);
    return true;
#elif defined(SIR_SYSLOG_ENABLED)
    int logopt = LOG_NDELAY | (ctx->include_pid ? LOG_PID : 0);
    int facility = LOG_USER;

    openlog(identity, logopt, facility);
    ctx->opened_log = true;
    _sir_selflog("%s: opened syslog(%s, %x, %x)\n", __func__, identity, logopt, facility);
    return true;
#endif
}

bool _sir_syslog_write(sir_level level, const sirbuf *buf, sir_syslog_dest *ctx) {
#if defined(SIR_OS_LOG_ENABLED)
    /*
        Value type      Custom specifier         Example output
        BOOL            %{BOOL}d                 YES
        bool            %{bool}d                 true
        darwin.errno    %{darwin.errno}d         [32: Broken pipe]
        darwin.mode     %{darwin.mode}d          drwxr-xr-x
        darwin.signal   %{darwin.signal}d        [sigsegv: Segmentation Fault]
        time_t          %{time_t}d               2016-01-12 19:41:37
        timeval         %{timeval}.*P            2016-01-12 19:41:37.774236
        timespec        %{timespec}.*P           2016-01-12 19:41:37.2382382823
        bytes           %{bytes}d                4.72 kB
        iec-bytes       %{iec-bytes}d            4.61 KiB
        bitrate         %{bitrate}d              123 kbps
        iec-bitrate     %{iec-bitrate}d          118 Kibps
        uuid_t          %{uuid_t}.16P            10742E39-0657-41F8-AB99-878C5EC2DCAA
        sockaddr        %{network:sockaddr}.*P   fe80::f:86ff:fee9:5c16
        in_addr         %{network:in_addr}d      127.0.0.1
        in6_addr        %{network:in6_addr}.16P  fe80::f:86ff:fee9:5c16
    */

    os_log(ctx->logger, "%s", buf->message);
    return true;
#elif defined(SIR_SYSLOG_ENABLED)
    int syslog_level = LOG_DEBUG;
    switch (level) {
        case SIRL_INFO:  syslog_level = LOG_INFO;    break;
        case SIRL_DEBUG: syslog_level = LOG_DEBUG;   break;
        case SIRL_WARN:  syslog_level = LOG_WARNING; break;
        case SIRL_ERROR: syslog_level = LOG_ERR;     break;
        case SIRL_CRIT:  syslog_level = LOG_CRIT;    break;
        case SIRL_ALERT: syslog_level = LOG_ALERT;   break;
        case SIRL_EMERG: syslog_level = LOG_EMERG;   break;
        default: /* this should never happen. */
            assert(!"invalid sir_level");
            return LOG_DEBUG;
    }

    syslog(syslog_level, "%s", buf->message);
    return true;
#endif
}

bool _sir_syslog_close(sir_syslog_dest *ctx) {
    if (!_sir_validptr(ctx))
        return false;

    if (!ctx->opened_log) {
        _sir_selflog("%s: system log not open; ignoring\n", __func__);
        return false;
    }

#if defined(SIR_OS_LOG_ENABLED)
    /* evidently, you don't need to close the handle returned from os_log_create(), and
     * if you make that call again, you'll get the same cached value. so let's keep the
     * value we've got in the global context. */
    ctx->opened_log = false;
    _sir_selflog("%s: system log closure not required\n", __func__);
    return true;
#elif (SIR_SYSLOG_ENABLED)
    closelog();
    ctx->opened_log = false;
    _sir_selflog("%s: closed system log\n", __func__);
    return true;
#endif
}

#else // SIR_NO_SYSTEM_LOGGERS

bool _sir_syslog_open(const char *name, sir_syslog_dest *ctx) {
    _SIR_UNUSED(name);
    _SIR_UNUSED(ctx);
    return false;
}

bool _sir_syslog_write(sir_level level, const sirbuf *buf, sir_syslog_dest *ctx) {
    _SIR_UNUSED(level);
    _SIR_UNUSED(buf);
    _SIR_UNUSED(ctx);
    return false;
}

bool _sir_syslog_close(sir_syslog_dest *ctx) {
    _SIR_UNUSED(ctx);
    return false;
}

#endif // !SIR_NO_SYSTEM_LOGGERS

const sirchar_t* _sir_levelstr(sir_level level) {
        
    if (_sir_validlevel(level)) {
        _SIR_DECLARE_BIN_SEARCH(0, _sir_countof(sir_level_str_map) - 1);
        _SIR_BEGIN_BIN_SEARCH();

        if (sir_level_str_map[_mid].level == level)
            return sir_level_str_map[_mid].str;

        int comparison = sir_level_str_map[_mid].level < level ? 1 : -1;

        _SIR_ITERATE_BIN_SEARCH(comparison);
        _SIR_END_BIN_SEARCH();
    }

    return SIR_UNKNOWN;
}

bool _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format) {

    if (0 != now && _sir_validptr(buffer) && _sir_validstr(format)) {
        struct tm timebuf = {0};
        size_t fmttime = strftime(buffer, SIR_MAXTIME, format, _sir_localtime(&now, &timebuf));
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
#if defined(SIR_MSEC_POSIX)
        struct timespec ts = {0};
        int clock          = clock_gettime(SIR_MSECCLOCK, &ts);
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
        static const ULONGLONG uepoch = (ULONGLONG)116444736e9;

        FILETIME ftutc = {0};
        GetSystemTimePreciseAsFileTime(&ftutc);

        ULARGE_INTEGER ftnow;
        ftnow.HighPart = ftutc.dwHighDateTime;
        ftnow.LowPart  = ftutc.dwLowDateTime;
        ftnow.QuadPart = (ULONGLONG)((ftnow.QuadPart - uepoch) / 1e7);

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
#if !defined(_WIN32)
    return getpid();
#else
    return (pid_t)GetProcessId(GetCurrentProcess());
#endif
}

pid_t _sir_gettid(void) {
    pid_t tid = 0;
#if defined(__MACOS__)
    uint64_t tid64 = 0;
    int gettid = pthread_threadid_np(NULL, &tid64);
    if (0 != gettid)
        _sir_handleerr(gettid);
    tid = (pid_t)tid64;
#elif defined(__BSD__)
    tid = (pid_t)pthread_getthreadid_np();
#elif defined(_DEFAULT_SOURCE)
    tid = syscall(SYS_gettid);
#elif defined(_WIN32)
    tid = (pid_t)GetCurrentThreadId();
#else
# error "cannot determine how to get thread id; please contact the author"
#endif
    return tid;
}

bool _sir_getthreadname(char name[SIR_MAXPID]) {
#if defined(__BSD__) || defined(_GNU_SOURCE)
    return 0 == pthread_getname_np(pthread_self(), name, SIR_MAXPID);
#else
    _SIR_UNUSED(name);
    return false;
#endif
}

/** @} */
