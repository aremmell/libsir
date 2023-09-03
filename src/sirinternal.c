/*
 * sirinternal.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.3
 * License:   The MIT License (MIT)
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
#include "sir/internal.h"
#include "sir/console.h"
#include "sir/defaults.h"
#include "sir/filecache.h"
#include "sir/plugins.h"
#include "sir/textstyle.h"
#include "sir/filesystem.h"
#include "sir/mutex.h"

#if defined(__WIN__)
# if defined(__EMBARCADEROC__) && defined(_WIN64)
#  pragma comment(lib, "ws2_32.a")
# else
#  pragma comment(lib, "ws2_32.lib")
# endif
#endif

static sirconfig _sir_cfg      = {0};
static sirfcache _sir_fc       = {0};
static sir_plugincache _sir_pc = {0};

static sir_mutex cfg_mutex = SIR_MUTEX_INIT;
static sir_once cfg_once   = SIR_ONCE_INIT;

static sir_mutex fc_mutex  = SIR_MUTEX_INIT;
static sir_once fc_once    = SIR_ONCE_INIT;

static sir_mutex pc_mutex  = SIR_MUTEX_INIT;
static sir_once pc_once    = SIR_ONCE_INIT;

static sir_mutex ts_mutex  = SIR_MUTEX_INIT;
static sir_once ts_once    = SIR_ONCE_INIT;

static sir_once magic_once = SIR_ONCE_INIT;

#if defined(__HAVE_ATOMIC_H__)
static atomic_uint_fast32_t _sir_magic;
#else
static volatile uint32_t _sir_magic;
#endif

static _sir_thread_local char _sir_tid[SIR_MAXPID] = {0};

bool _sir_makeinit(sirinit* si) {
    bool retval = _sir_validptr(si);

    if (retval) {
        memset(si, 0, sizeof(sirinit));

        si->d_stdout.opts   = SIRO_DEFAULT;
        si->d_stdout.levels = SIRL_DEFAULT;

        si->d_stderr.opts   = SIRO_DEFAULT;
        si->d_stderr.levels = SIRL_DEFAULT;

#if !defined(SIR_NO_SYSTEM_LOGGERS)
        si->d_syslog.opts   = SIRO_DEFAULT;
        si->d_syslog.levels = SIRL_DEFAULT;
#else
        si->d_syslog.opts   = SIRO_MSGONLY;
        si->d_syslog.levels = SIRL_NONE;
#endif
    }

    return retval;
}

bool _sir_init(sirinit* si) {
    (void)_sir_seterror(_SIR_E_NOERROR);
    _sir_once(&magic_once, _sir_initialize_once);

    if (!_sir_validptr(si))
        return false;

#if defined(__HAVE_ATOMIC_H__)
    if (_SIR_MAGIC == atomic_load(&_sir_magic))
#else
    if (_SIR_MAGIC == _sir_magic)
#endif
        return _sir_seterror(_SIR_E_ALREADY);

    _sir_defaultlevels(&si->d_stdout.levels, sir_stdout_def_lvls);
    _sir_defaultopts(&si->d_stdout.opts, sir_stdout_def_opts);

    _sir_defaultlevels(&si->d_stderr.levels, sir_stderr_def_lvls);
    _sir_defaultopts(&si->d_stderr.opts, sir_stderr_def_opts);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_defaultlevels(&si->d_syslog.levels, sir_syslog_def_lvls);
    _sir_defaultopts(&si->d_syslog.opts, sir_syslog_def_opts);
#endif

    if (!_sir_init_sanity(si))
        return false;

    _SIR_LOCK_SECTION(sirconfig, _cfg, SIRMI_CONFIG, false);

#if !defined(__WIN__)
    tzset();
#endif

#if defined(__HAVE_ATOMIC_H__)
    atomic_store(&_sir_magic, _SIR_MAGIC);
#else
    _sir_magic = _SIR_MAGIC;
#endif

#if defined(__WIN__)
    _sir_initialize_stdio();
#endif

    _sir_setcolormode(SIRCM_16);

    if (!_sir_resettextstyles())
        _sir_selflog("error: failed to reset text styles!");

    memset(&_cfg->state, 0, sizeof(_cfg->state));
    memcpy(&_cfg->si, si, sizeof(sirinit));

    /* forcibly null-terminate the process name. */
    _cfg->si.name[SIR_MAXNAME - 1] = '\0';

    /* Store PID. */
    _cfg->state.pid = _sir_getpid();

    (void)snprintf(_cfg->state.pidbuf, SIR_MAXPID, SIR_PIDFORMAT,
        PID_CAST _cfg->state.pid);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    /* initialize system logger. */
    _sir_syslog_reset(&_cfg->si.d_syslog);

    if (_cfg->si.d_syslog.levels != SIRL_NONE) {
        if (!_sir_syslog_init(_cfg->si.name, &_cfg->si.d_syslog))
            _sir_selflog("failed to initialize system logger!");
    }
#endif

    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    return true;
}

bool _sir_cleanup(void) {
    if (!_sir_sanity())
        return false;

    _SIR_LOCK_SECTION(sirfcache, sfc, SIRMI_FILECACHE, false);
    bool cleanup   = true;
    bool destroyfc = _sir_fcache_destroy(sfc);
    SIR_ASSERT(destroyfc);

    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);
    cleanup &= destroyfc;

#if !defined(SIR_NO_PLUGINS)
    _SIR_LOCK_SECTION(sir_plugincache, spc, SIRMI_PLUGINCACHE, false);
    bool destroypc = _sir_plugin_cache_destroy(spc);
    SIR_ASSERT(destroypc);
    _SIR_UNLOCK_SECTION(SIRMI_PLUGINCACHE);
    cleanup &= destroypc;
#endif

    _SIR_LOCK_SECTION(sirconfig, _cfg, SIRMI_CONFIG, false);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_syslog_close(&_cfg->si.d_syslog)) {
        cleanup = false;
        _sir_selflog("error: failed to close system logger!");
    }

    _sir_syslog_reset(&_cfg->si.d_syslog);
#endif

    if (!_sir_resettextstyles()) {
        cleanup = false;
        _sir_selflog("error: failed to reset text styles!");
    }

#if defined(__HAVE_ATOMIC_H__)
    atomic_store(&_sir_magic, 0);
#else
    _sir_magic = 0U;
#endif

    memset(_cfg, 0, sizeof(sirconfig));
    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    _sir_selflog("cleanup: %s", (cleanup ? "successful" : "with errors"));

    SIR_ASSERT(cleanup);
    return cleanup;
}

bool _sir_sanity(void) {
#if defined(__HAVE_ATOMIC_H__)
    if (_SIR_MAGIC == atomic_load(&_sir_magic))
        return true;
#else
    if (_SIR_MAGIC == _sir_magic)
        return true;
#endif
    return _sir_seterror(_SIR_E_NOTREADY);
}

bool _sir_init_sanity(const sirinit* si) {
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

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    optscheck &= _sir_validopts(si->d_syslog.opts);
#endif

    return levelcheck && optscheck;
}

static
bool _sir_updatelevels(const char* name, sir_levels* old, sir_levels* new) {
    if (*old != *new) {
        _sir_selflog("updating %s levels from %04"PRIx16" to %04"PRIx16, name, *old, *new);
        *old = *new;
    } else {
        _sir_selflog("skipped superfluous update of %s levels: %04"PRIx16, name, *old);
    }
    return true;
}

static
bool _sir_updateopts(const char* name, sir_options* old, sir_options* new) {
    if (*old != *new) {
        _sir_selflog("updating %s options from %08"PRIx32" to %08"PRIx32, name, *old, *new);
        *old = *new;
    } else {
        _sir_selflog("skipped superfluous update of %s options: %08"PRIx32, name, *old);
    }
    return true;
}

bool _sir_stdoutlevels(sirinit* si, sir_update_config_data* data) {
    return _sir_updatelevels(SIR_DESTNAME_STDOUT, &si->d_stdout.levels, data->levels);
}

bool _sir_stdoutopts(sirinit* si, sir_update_config_data* data) {
    return _sir_updateopts(SIR_DESTNAME_STDOUT, &si->d_stdout.opts, data->opts);
}

bool _sir_stderrlevels(sirinit* si, sir_update_config_data* data) {
    return _sir_updatelevels(SIR_DESTNAME_STDERR, &si->d_stderr.levels, data->levels);
}

bool _sir_stderropts(sirinit* si, sir_update_config_data* data) {
    return _sir_updateopts(SIR_DESTNAME_STDERR, &si->d_stderr.opts, data->opts);
}

bool _sir_sysloglevels(sirinit* si, sir_update_config_data* data) {
    bool updated = _sir_updatelevels(SIR_DESTNAME_SYSLOG, &si->d_syslog.levels, data->levels);
    if (updated) {
        _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_LEVELS);
        updated = _sir_syslog_updated(si, data);
        _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_LEVELS);
    }
    return updated;
}

bool _sir_syslogopts(sirinit* si, sir_update_config_data* data) {
    bool updated = _sir_updateopts(SIR_DESTNAME_SYSLOG, &si->d_syslog.opts, data->opts);
    if (updated) {
        _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_OPTIONS);
        updated = _sir_syslog_updated(si, data);
        _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_OPTIONS);
    }
    return updated;
}

bool _sir_syslogid(sirinit* si, sir_update_config_data* data) {
    bool cur_valid = _sir_validstrnofail(si->d_syslog.identity);
    if (!cur_valid || 0 != strncmp(si->d_syslog.identity, data->sl_identity, SIR_MAX_SYSLOG_ID)) {
        _sir_selflog("updating %s identity from '%s' to '%s'", SIR_DESTNAME_SYSLOG,
            si->d_syslog.identity, data->sl_identity);
        _sir_strncpy(si->d_syslog.identity, SIR_MAX_SYSLOG_ID, data->sl_identity,
            strnlen(data->sl_identity, SIR_MAX_SYSLOG_ID));
    } else {
        _sir_selflog("skipped superfluous update of %s identity: '%s'", SIR_DESTNAME_SYSLOG,
            si->d_syslog.identity);
        return true;
    }

    _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_IDENTITY);
    bool updated = _sir_syslog_updated(si, data);
    _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_IDENTITY);

    return updated;
}

bool _sir_syslogcat(sirinit* si, sir_update_config_data* data) {
    bool cur_valid = _sir_validstrnofail(si->d_syslog.category);
    if (!cur_valid || 0 != strncmp(si->d_syslog.category, data->sl_category, SIR_MAX_SYSLOG_CAT)) {
        _sir_selflog("updating %s category from '%s' to '%s'", SIR_DESTNAME_SYSLOG,
            si->d_syslog.category, data->sl_category);
        _sir_strncpy(si->d_syslog.category, SIR_MAX_SYSLOG_CAT, data->sl_category,
            strnlen(data->sl_category, SIR_MAX_SYSLOG_CAT));
    } else {
        _sir_selflog("skipped superfluous update of %s category: '%s'", SIR_DESTNAME_SYSLOG,
            si->d_syslog.identity);
        return true;
    }

    _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_CATEGORY);
    bool updated = _sir_syslog_updated(si, data);
    _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_CATEGORY);

    return updated;
}

bool _sir_writeinit(sir_update_config_data* data, sirinit_update update) {
    (void)_sir_seterror(_SIR_E_NOERROR);

    if (!_sir_sanity() || !_sir_validupdatedata(data) || !_sir_validfnptr(update))
        return false;

    _SIR_LOCK_SECTION(sirconfig, _cfg, SIRMI_CONFIG, false);

    bool updated = update(&_cfg->si, data);
    if (!updated)
        _sir_selflog("error: update routine failed!");

    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);
    return updated;
}

void* _sir_locksection(sir_mutex_id mid) {
    sir_mutex* m = NULL;
    void* sec    = NULL;

    bool enter = _sir_mapmutexid(mid, &m, &sec) && _sir_mutexlock(m);
    SIR_ASSERT(enter);

    return enter ? sec : NULL;
}

void _sir_unlocksection(sir_mutex_id mid) {
    sir_mutex* m = NULL;
    void* sec    = NULL;

    bool leave = _sir_mapmutexid(mid, &m, &sec) && _sir_mutexunlock(m);
    SIR_ASSERT_UNUSED(leave, leave);
}

bool _sir_mapmutexid(sir_mutex_id mid, sir_mutex** m, void** section) {
    sir_mutex* tmpm;
    void* tmpsec;

    switch (mid) {
        case SIRMI_CONFIG:
            _sir_once(&cfg_once, _sir_initmutex_cfg_once);
            tmpm   = &cfg_mutex;
            tmpsec = &_sir_cfg;
            break;
        case SIRMI_FILECACHE:
            _sir_once(&fc_once, _sir_initmutex_fc_once);
            tmpm   = &fc_mutex;
            tmpsec = &_sir_fc;
            break;
        case SIRMI_PLUGINCACHE:
            _sir_once(&pc_once, _sir_initmutex_pc_once);
            tmpm   = &pc_mutex;
            tmpsec = &_sir_pc;
            break;
        case SIRMI_TEXTSTYLE:
            _sir_once(&ts_once, _sir_initmutex_ts_once);
            tmpm   = &ts_mutex;
            tmpsec = &sir_text_style_section;
            break;
        // GCOVR_EXCL_START
        default: /* this should never happen. */
            SIR_ASSERT(mid);
            tmpm   = NULL;
            tmpsec = NULL;
            break;
        // GCOVR_EXCL_STOP
    }

    *m = tmpm;

    if (section)
        *section = tmpsec;

    return *m != NULL && (!section || *section != NULL);
}

#if !defined(__WIN__)
void _sir_initialize_once(void) {
# if defined(__HAVE_ATOMIC_H__)
    atomic_init(&_sir_magic, 0);
# endif
}

void _sir_initmutex_cfg_once(void) {
    bool created = _sir_mutexcreate(&cfg_mutex);
    SIR_ASSERT_UNUSED(created, created);
}

void _sir_initmutex_fc_once(void) {
    bool created = _sir_mutexcreate(&fc_mutex);
    SIR_ASSERT_UNUSED(created, created);
}

void _sir_initmutex_pc_once(void) {
    bool created = _sir_mutexcreate(&pc_mutex);
    SIR_ASSERT_UNUSED(created, created);
}

void _sir_initmutex_ts_once(void) {
    bool created = _sir_mutexcreate(&ts_mutex);
    SIR_ASSERT_UNUSED(created, created);
}

#else /* __WIN__ */
BOOL CALLBACK _sir_initialize_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);
    return TRUE;
}

BOOL CALLBACK _sir_initmutex_cfg_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);

    if (!_sir_mutexcreate(&cfg_mutex)) {
        SIR_ASSERT(!"failed to create mutex!");
        return FALSE;
    }

    return TRUE;
}

BOOL CALLBACK _sir_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);

    if (!_sir_mutexcreate(&fc_mutex)) {
        SIR_ASSERT(!"failed to create mutex!");
        return FALSE;
    }

    return TRUE;
}

BOOL CALLBACK _sir_initmutex_pc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);

    if (!_sir_mutexcreate(&pc_mutex)) {
        SIR_ASSERT(!"failed to create mutex!");
        return FALSE;
    }

    return TRUE;
}

BOOL CALLBACK _sir_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);

    if (!_sir_mutexcreate(&ts_mutex)) {
        SIR_ASSERT(!"failed to create mutex!");
        return FALSE;
    }

    return TRUE;
}
#endif

bool _sir_once(sir_once* once, sir_once_fn func) {
#if !defined(__WIN__)
    int ret = pthread_once(once, func);
    return 0 == ret ? true : _sir_handleerr(ret);
#else /* __WIN__ */
    return (FALSE != InitOnceExecuteOnce(once, func, NULL, NULL)) ? true
        : _sir_handlewin32err(GetLastError());
#endif
}

PRINTF_FORMAT_ATTR(2, 0)
bool _sir_logv(sir_level level, PRINTF_FORMAT const char* format, va_list args) {
    if (!_sir_sanity() || !_sir_validlevel(level) || !_sir_validstr(format))
        return false;

    (void)_sir_seterror(_SIR_E_NOERROR);

    _SIR_LOCK_SECTION(sirconfig, _cfg, SIRMI_CONFIG, false);

    sirbuf buf = {0};

    /* from time to time, update the host name in the config, just in case. */
    time_t now_sec = -1;
    if (-1 != time(&now_sec) &&
        (now_sec - _cfg->state.last_hname_chk) > SIR_HNAME_CHK_INTERVAL) { //-V522
        _sir_selflog("updating hostname...");
        if (!_sir_gethostname(_cfg->state.hostname)) {
            _sir_selflog("error: failed to get hostname!");
        } else {
            _cfg->state.last_hname_chk = now_sec;
            _sir_selflog("hostname: '%s'", _cfg->state.hostname);
        }
    }

    sir_time time_chk;
    int64_t msec_since_time_chk = _sir_msec_since(_cfg->state.last_time_chk.sec,
        _cfg->state.last_time_chk.msec, &time_chk);

    long now_msec = 0L;
    bool gettime = _sir_clock_gettime(SIR_WALLCLOCK, &now_sec, &now_msec);
    SIR_ASSERT_UNUSED(getttime, gettime);

    /* update milliseconds. */
    _sir_snprintf_trunc(buf.msec, SIR_MAXMSEC, SIR_MSECFORMAT, now_msec);

    /* update the string form of the timestamp (h/m/s) and the thread identifier/name
     * if SIR_TIME_CHK_INTERVAL milliseconds have elapsed since the last update. */
    if (msec_since_time_chk > SIR_TIME_CHK_INTERVAL) {
        _cfg->state.last_time_chk = time_chk;

        /* update hours/minutes/seconds. */
        bool fmt = _sir_formattime(now_sec, _cfg->state.timestamp, SIR_TIMEFORMAT);
        SIR_ASSERT_UNUSED(fmt, fmt);

        sir_time thrd_chk;
        int64_t msec_since_thrd_chk = _sir_msec_since(_cfg->state.last_thrd_chk.sec,
            _cfg->state.last_thrd_chk.msec, &thrd_chk);

        if (msec_since_thrd_chk > SIR_THRD_CHK_INTERVAL) {
            /* update the thread identifier/name. decide how to identify this
             * thread based on the configuration, and whether or not its identifier
             * is identical to the process identifier. */
            _cfg->state.last_thrd_chk = thrd_chk;

            pid_t tid         = _sir_gettid();
            bool resolved_tid = false;

            if (tid == _cfg->state.pid) {
#if SIR_DUPE_THREAD_ID_USE_NAME
                /* if a name is set, use it. */
                resolved_tid = _sir_getthreadname(_sir_tid);
#else
                /* don't use anything to identify the thread. */
                _sir_resetstr(_sir_tid);
                resolved_tid = true;
#endif
            }

#if !SIR_PREFER_THREAD_ID
            if (!resolved_tid)
                resolved_tid = _sir_getthreadname(_sir_tid);
#endif

            if (!resolved_tid)
                _sir_snprintf_trunc(_sir_tid, SIR_MAXPID, SIR_PIDFORMAT,
                    PID_CAST tid);
        }
    }

    sirconfig cfg;
    memcpy(&cfg, _cfg, sizeof(sirconfig));
    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    buf.timestamp = cfg.state.timestamp;
    buf.hostname  = cfg.state.hostname;
    buf.pid       = cfg.state.pidbuf;
    buf.name      = cfg.si.name;

    const char* style_str = _sir_gettextstyle(level);

    SIR_ASSERT(NULL != style_str);
    if (NULL != style_str)
        (void)_sir_strncpy(buf.style, SIR_MAXSTYLE, style_str,
            strnlen(style_str, SIR_MAXSTYLE));

    buf.level = _sir_formattedlevelstr(level);

    (void)_sir_strncpy(buf.tid, SIR_MAXPID, _sir_tid, SIR_MAXPID);

    (void)vsnprintf(buf.message, SIR_MAXMESSAGE, format, args);

    if (!_sir_validstr(buf.message))
        return _sir_seterror(_SIR_E_INTERNAL);

    bool match             = false;
    bool exit_early        = false;
    bool update_last_props = true;
    uint64_t hash          = 0ULL;

    if (cfg.state.last.prefix[0] == buf.message[0]  &&
        cfg.state.last.prefix[1] == buf.message[1]) {
        hash  = FNV64_1a(buf.message);
        match = cfg.state.last.hash == hash;
    }

    if (match) {
        cfg.state.last.counter++;

        /* _sir_selflog("message '%s' matches last; incremented counter to %zu", buf.message,
            cfg.state.last.counter); */

        if (cfg.state.last.counter >= cfg.state.last.threshold - 2) {
            size_t old_threshold = cfg.state.last.threshold;

            update_last_props = false;
            cfg.state.last.threshold *= SIR_SQUELCH_BACKOFF_FACTOR;
            cfg.state.last.squelch = true;

            _sir_selflog("hit squelch threshold of %zu; setting new threshold"
                         " to %zu (factor: %d)",
                old_threshold, cfg.state.last.threshold, SIR_SQUELCH_BACKOFF_FACTOR);

            (void)snprintf(buf.message, SIR_MAXMESSAGE, SIR_SQUELCH_MSG_FORMAT, old_threshold);
        } else if (cfg.state.last.squelch) {
            exit_early = true;
        }
    } else {
        cfg.state.last.squelch   = false;
        cfg.state.last.counter   = 0;
        cfg.state.last.threshold = SIR_SQUELCH_THRESHOLD;
        /* _sir_selflog("message '%s' does not match last; resetting", buf.message); */
    }

    _cfg = _sir_locksection(SIRMI_CONFIG);
    if (!_cfg)
        return _sir_seterror(_SIR_E_INTERNAL);

    _cfg->state.last.squelch = cfg.state.last.squelch;

    if (update_last_props) {
        _cfg->state.last.hash      = hash;
        _cfg->state.last.prefix[0] = buf.message[0];
        _cfg->state.last.prefix[1] = buf.message[1];
    }

    _cfg->state.last.counter   = cfg.state.last.counter;
    _cfg->state.last.threshold = cfg.state.last.threshold;

    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    if (exit_early)
        return false;

    bool dispatched = _sir_dispatch(&cfg.si, level, &buf);
    return update_last_props ? dispatched : false;
}

bool _sir_dispatch(sirinit* si, sir_level level, sirbuf* buf) {
    bool retval       = true;
    size_t dispatched = 0;
    size_t wanted     = 0;

    if (_sir_bittest(si->d_stdout.levels, level)) {
        const char* write = _sir_format(true, si->d_stdout.opts, buf);
        bool wrote        = _sir_validstrnofail(write) &&
            _sir_write_stdout(write, buf->output_len);
        retval &= wrote;

        if (wrote)
            dispatched++;
        wanted++;
    }

    if (_sir_bittest(si->d_stderr.levels, level)) {
        const char* write = _sir_format(true, si->d_stderr.opts, buf);
        bool wrote        = _sir_validstrnofail(write) &&
            _sir_write_stderr(write, buf->output_len);
        retval &= wrote;

        if (wrote)
            dispatched++;
        wanted++;
    }

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (_sir_bittest(si->d_syslog.levels, level)) {
        if (_sir_syslog_write(level, buf, &si->d_syslog))
            dispatched++;
        wanted++;
    }
#endif

    _SIR_LOCK_SECTION(sirfcache, sfc, SIRMI_FILECACHE, false);
    size_t fdispatched = 0;
    size_t fwanted     = 0;
    retval &= _sir_fcache_dispatch(sfc, level, buf, &fdispatched, &fwanted);
    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);

    dispatched += fdispatched;
    wanted += fwanted;

#if !defined(SIR_NO_PLUGINS)
    _SIR_LOCK_SECTION(sir_plugincache, spc, SIRMI_PLUGINCACHE, false);
    size_t pdispatched = 0;
    size_t pwanted     = 0;
    retval &= _sir_plugin_cache_dispatch(spc, level, buf, &pdispatched, &pwanted);
    _SIR_UNLOCK_SECTION(SIRMI_PLUGINCACHE);

    dispatched += pdispatched;
    wanted += pwanted;
#endif

    if (0 == wanted) {
        _sir_selflog("error: no destinations registered for level %04"PRIx16, level);
        return _sir_seterror(_SIR_E_NODEST);
    }

    return retval && (dispatched == wanted);
}

const char* _sir_format(bool styling, sir_options opts, sirbuf* buf) {
    if (_sir_validptr(buf)) {
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

        if (!_sir_bittest(opts, SIRO_NOHOST) && _sir_validstrnofail(buf->hostname)) {
            if (!first)
                _sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);
            _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->hostname, SIR_MAXHOST);
            first = false;
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
                _sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_PIDPREFIX, 1);
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
                _sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_PIDSUFFIX, 1);

            if (first)
                first = false;
        }

        if (!first)
            _sir_strncat(buf->output, SIR_MAXOUTPUT, ": ", 2);

        _sir_strncat(buf->output, SIR_MAXOUTPUT, buf->message, SIR_MAXMESSAGE);

        if (styling)
            _sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_ESC_RST, SIR_MAXSTYLE);

        _sir_strncat(buf->output, SIR_MAXOUTPUT, "\n", 1);

        buf->output_len = strnlen(buf->output, SIR_MAXOUTPUT);

        return buf->output;
    }

    return NULL;
}

bool _sir_syslog_init(const char* name, sir_syslog_dest* ctx) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_validptr(name) || !_sir_validptr(ctx))
        return false;

    /* begin resolve identity. */
    if (!_sir_validstrnofail(ctx->identity)) {
        _sir_selflog("ctx->identity is no good; trying name");
        if (_sir_validstrnofail(name)) {
            _sir_selflog("using name");
            _sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, name, strnlen(name, SIR_MAX_SYSLOG_ID));
        } else {
            _sir_selflog("name is no good; trying filename");
            char* appbasename = _sir_getappbasename();
            if (_sir_validstrnofail(appbasename)) {
                _sir_selflog("filename is good: %s", appbasename);
                _sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, appbasename,
                    strnlen(appbasename, SIR_MAX_SYSLOG_ID));
            } else {
                _sir_selflog("filename no good; using fallback");
                _sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, SIR_FALLBACK_SYSLOG_ID,
                    strnlen(SIR_FALLBACK_SYSLOG_ID, SIR_MAX_SYSLOG_ID));
            }
            _sir_safefree(&appbasename);
        }
    } else {
        _sir_selflog("already have identity");
    }

    /* category */
    if (!_sir_validstrnofail(ctx->category)) {
        _sir_selflog("category not set; using fallback");
        _sir_strncpy(ctx->category, SIR_MAX_SYSLOG_CAT, SIR_FALLBACK_SYSLOG_CAT,
            strnlen(SIR_FALLBACK_SYSLOG_CAT, SIR_MAX_SYSLOG_CAT));
    } else {
        _sir_selflog("already have category");
    }

    _sir_setbitshigh(&ctx->_state.mask, SIRSL_IS_INIT);
    _sir_selflog("resolved (identity: '%s', category: '%s')", ctx->identity, ctx->category);

    return _sir_syslog_open(ctx);
#else
    SIR_UNUSED(name);
    SIR_UNUSED(ctx);
    return false;
#endif
}

bool _sir_syslog_open(sir_syslog_dest* ctx) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_INIT)) {
        _sir_selflog("not initialized; ignoring");
        return _sir_seterror(_SIR_E_INVALID);
    }

    if (_sir_bittest(ctx->_state.mask, SIRSL_IS_OPEN)) {
        _sir_selflog("log already open; ignoring");
        return true;
    }

    _sir_selflog("opening log (levels: %04"PRIx16", options: %08"PRIx32")", ctx->levels,
        ctx->opts);

# if defined(SIR_OS_LOG_ENABLED)
    ctx->_state.logger = (void*)os_log_create(ctx->identity, ctx->category);
    _sir_selflog("opened os_log ('%s', '%s')", ctx->identity, ctx->category);
# elif defined(SIR_SYSLOG_ENABLED)
    int logopt   = LOG_NDELAY | (_sir_bittest(ctx->opts, SIRO_NOPID) ? 0 : LOG_PID);
    int facility = LOG_USER;

    openlog(ctx->identity, logopt, facility);
    _sir_selflog("opened syslog('%s', %x, %x)", ctx->identity, logopt, facility);
# endif

    _sir_setbitshigh(&ctx->_state.mask, SIRSL_IS_OPEN);
    return true;
#else
    SIR_UNUSED(ctx);
    return false;
#endif
}

bool _sir_syslog_write(sir_level level, const sirbuf* buf, sir_syslog_dest* ctx) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_INIT)) {
        _sir_selflog("not initialized; ignoring");
        return _sir_seterror(_SIR_E_INVALID);
    }

    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_OPEN)) {
        _sir_selflog("log not open; ignoring");
        return _sir_seterror(_SIR_E_INVALID);
    }

# if defined(SIR_OS_LOG_ENABLED)
    if (SIRL_DEBUG == level)
        os_log_debug((os_log_t)ctx->_state.logger, SIR_OS_LOG_FORMAT, buf->message);
    else if (SIRL_INFO == level || SIRL_NOTICE == level)
        os_log_info((os_log_t)ctx->_state.logger, SIR_OS_LOG_FORMAT, buf->message);
    else if (SIRL_WARN == level || SIRL_ERROR == level)
        os_log_error((os_log_t)ctx->_state.logger, SIR_OS_LOG_FORMAT, buf->message);
    else if (SIRL_ALERT == level || SIRL_CRIT == level || SIRL_EMERG == level)
        os_log_fault((os_log_t)ctx->_state.logger, SIR_OS_LOG_FORMAT, buf->message);

    return true;
# elif defined(SIR_SYSLOG_ENABLED)
    int syslog_level;
    switch (level) {
        case SIRL_DEBUG:  syslog_level = LOG_DEBUG; break;
        case SIRL_INFO:   syslog_level = LOG_INFO; break;
        case SIRL_NOTICE: syslog_level = LOG_NOTICE; break;
        case SIRL_WARN:   syslog_level = LOG_WARNING; break;
        case SIRL_ERROR:  syslog_level = LOG_ERR; break;
        case SIRL_CRIT:   syslog_level = LOG_CRIT; break;
        case SIRL_ALERT:  syslog_level = LOG_ALERT; break;
        case SIRL_EMERG:  syslog_level = LOG_EMERG; break;
        // GCOVR_EXCL_START
        case SIRL_NONE: /* this should never happen. */
        default:
            SIR_ASSERT(level);
            syslog_level = LOG_DEBUG;
        // GCOVR_EXCL_STOP
    }

    syslog(syslog_level, "%s", buf->message);
    return true;
# endif
#else
    SIR_UNUSED(level);
    SIR_UNUSED(buf);
    SIR_UNUSED(ctx);
    return false;
#endif
}

bool _sir_syslog_updated(sirinit* si, sir_update_config_data* data) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_validptr(si) || !_sir_validptr(data))
        return false;

    if (_sir_bittest(si->d_syslog._state.mask, SIRSL_UPDATED)) {
        bool levels   = _sir_bittest(si->d_syslog._state.mask, SIRSL_LEVELS);
        bool options  = _sir_bittest(si->d_syslog._state.mask, SIRSL_OPTIONS);
        bool category = _sir_bittest(si->d_syslog._state.mask, SIRSL_CATEGORY);
        bool identity = _sir_bittest(si->d_syslog._state.mask, SIRSL_IDENTITY);
        bool is_init  = _sir_bittest(si->d_syslog._state.mask, SIRSL_IS_INIT);
        bool is_open  = _sir_bittest(si->d_syslog._state.mask, SIRSL_IS_OPEN);

        _sir_selflog("config update: (levels: %u, options: %u, category: %u,"
                     " identity: %u, is_init: %u, is_open: %u)",
                     levels, options, category, identity, is_init, is_open);

        bool must_init = false;

# if defined(SIR_OS_LOG_ENABLED)
        /*
         * for os_log, if initialized and open already, only need to reconfigure
         * if identity or category changed.
         */
        must_init = (!is_init || !is_open) || (identity || category);
# elif defined(SIR_SYSLOG_ENABLED)
        /*
         * for os_log, if initialized and open already, only need to reconfigure
         * if identity or options changed.
         */
        must_init = (!is_init || !is_open) || (identity || options);
# endif
        bool init = true;
        if (must_init) {
            _sir_selflog("re-init...");
            init = _sir_syslog_init(si->name, &si->d_syslog);
            _sir_selflog("re-init %s", init ? "succeeded" : "failed");
        } else {
            _sir_selflog("no re-init necessary");
        }

        return init;
    } else {
        _sir_selflog("BUG: called without 'updated' flag set!");
        return false;
    }
#else
    SIR_UNUSED(si);
    SIR_UNUSED(data);
    return false;
#endif
}

bool _sir_syslog_close(sir_syslog_dest* ctx) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_validptr(ctx))
        return false;

    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_OPEN)) {
        _sir_selflog("log not open; ignoring");
        return true;
    }

# if defined(SIR_OS_LOG_ENABLED)
    /* evidently, you don't need to close the handle returned from os_log_create(), and
     * if you make that call again, you'll get the same cached value. so let's keep the
     * value we've got in the global context. */
    _sir_setbitslow(&ctx->_state.mask, SIRSL_IS_OPEN);
    _sir_selflog("log closure not necessary");
    return true;
# elif defined(SIR_SYSLOG_ENABLED)
    closelog();
    _sir_setbitslow(&ctx->_state.mask, SIRSL_IS_OPEN);
    _sir_selflog("closed log");
    return true;
# endif
#else
    SIR_UNUSED(ctx);
    return false;
#endif
}

void _sir_syslog_reset(sir_syslog_dest* ctx) {
#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (_sir_validptr(ctx)) {
        uint32_t old       = ctx->_state.mask;
        ctx->_state.mask   = 0U;
        ctx->_state.logger = NULL;
        _sir_selflog("state reset; mask was %08"PRIx32, old);
    }
#else
    SIR_UNUSED(ctx);
#endif
}

const char* _sir_formattedlevelstr(sir_level level) {
    static const size_t low  = 0;
    static const size_t high = SIR_NUMLEVELS - 1;

    const char* retval = SIR_UNKNOWN;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH()

    if (sir_level_to_str_map[_mid].level == level) {
        retval = sir_level_to_str_map[_mid].fmt;
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((sir_level_to_str_map[_mid].level < level ? 1 : -1));
    _SIR_END_BIN_SEARCH();

    return retval;
}

bool _sir_clock_gettime(int clock, time_t* tbuf, long* msecbuf) {
    if (tbuf) {
#if defined(SIR_MSEC_POSIX)
        struct timespec ts = {0};
        int ret            = clock_gettime(clock, &ts);
        SIR_ASSERT(0 == ret);

        if (0 == ret) {
            *tbuf = ts.tv_sec;
            if (msecbuf)
                *msecbuf = (long)(ts.tv_nsec / 1000000L);
        } else {
            if (msecbuf)
                *msecbuf = 0L;
            return _sir_handleerr(errno);
        }
#elif defined(SIR_MSEC_WIN32)
        SIR_UNUSED(clock);
        static const ULONGLONG uepoch = (ULONGLONG)116444736e9;

        FILETIME ftutc = {0};
        GetSystemTimePreciseAsFileTime(&ftutc);

        ULARGE_INTEGER ftnow = {0};
        ftnow.HighPart = ftutc.dwHighDateTime;
        ftnow.LowPart  = ftutc.dwLowDateTime;
        ftnow.QuadPart = (ULONGLONG)((ftnow.QuadPart - uepoch) / 10000000ULL);

        *tbuf = (time_t)ftnow.QuadPart;

        SYSTEMTIME st = {0};
        if (FileTimeToSystemTime(&ftutc, &st)) {
            if (msecbuf)
                *msecbuf = (long)st.wMilliseconds;
        } else {
            if (msecbuf)
                *msecbuf = 0L;
            return _sir_handlewin32err(GetLastError());
        }
#else
        time(tbuf);
        if (msecbuf)
            *msecbuf = 0L;
#endif
        return true;
    }
    return false;
}

int64_t _sir_msec_since(time_t when_sec, long when_msec, sir_time* out) {
    if (!_sir_validptr(out))
        return 0LL;

    out->sec  = 0;
    out->msec = 0L;

    bool gettime = _sir_clock_gettime(SIR_INTERVALCLOCK, &out->sec, &out->msec);
    SIR_ASSERT(gettime);

    if (!gettime || (out->sec < when_sec))
        return 0LL;

    return ((((int64_t)out->sec) * 1000LL) + ((int64_t)out->msec)) -
           ((((int64_t)when_sec) * 1000LL) + ((int64_t)when_msec));
}

pid_t _sir_getpid(void) {
#if !defined(__WIN__)
    return getpid();
#else /* __WIN__ */
    return (pid_t)GetCurrentProcessId();
#endif
}

pid_t _sir_gettid(void) {
    pid_t tid = 0;
#if defined(__MACOS__)
    uint64_t tid64 = 0ULL;
    int gettid     = pthread_threadid_np(NULL, &tid64);
    if (0 != gettid)
        (void)_sir_handleerr(gettid);
    tid = (pid_t)tid64;
#elif (defined(__BSD__) && !defined(__NetBSD__) && !defined(__OpenBSD__)) || \
      defined(__DragonFly_getthreadid__)
    tid = (pid_t)pthread_getthreadid_np();
#elif defined(__OpenBSD__)
    tid = (pid_t)getthrid();
#elif defined(__SOLARIS__) || defined(__NetBSD__) || defined(__HURD__) || \
      defined(__DragonFly__) || defined(__CYGWIN__) || defined(_AIX)
    tid = (pid_t)pthread_self();
#elif defined(__HAIKU__)
    tid = get_pthread_thread_id(pthread_self());
#elif defined(__linux__) || defined(__serenity__)
# if (defined(__GLIBC__) && GLIBC_VERSION >= 23000) || \
      defined(__serenity__)
    tid = gettid();
# else
    tid = syscall(SYS_gettid);
# endif
#elif defined(__WIN__)
    tid = (pid_t)GetCurrentThreadId();
#else
# error "cannot determine how to get thread id; please contact the author"
#endif
    return tid;
}

bool _sir_getthreadname(char name[SIR_MAXPID]) {
    _sir_resetstr(name);
#if defined(__MACOS__) || \
   (defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_12_2__)) || \
   (defined(__GLIBC__) && GLIBC_VERSION >= 21200 && defined(_GNU_SOURCE)) || \
    defined(USE_PTHREAD_GETNAME_NP)
    int ret = pthread_getname_np(pthread_self(), name, SIR_MAXPID);
    if (0 != ret)
        return _sir_handleerr(ret);
# if defined(__HAIKU__)
    if ((strncmp(name, "pthread_func", SIR_MAXPID)) || _sir_validstrnofail(name))
        (void)snprintf(name, SIR_MAXPID, "%ld", (long)get_pthread_thread_id(pthread_self()));
# endif
    return _sir_validstrnofail(name);
#elif defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_11_3__)
    pthread_get_name_np(pthread_self(), name, SIR_MAXPID);
    return _sir_validstrnofail(name);
#else
# if !defined(_AIX) && !defined(__HURD__) && !defined(SUNLINT)
#  pragma message("unable to determine how to get a thread name")
# endif
    SIR_UNUSED(name);
    return false;
#endif
}

bool _sir_setthreadname(const char* name) {
    if (!_sir_validptr(name))
        return false;
#if defined (__MACOS__)
    int ret = pthread_setname_np(name);
    return (0 != ret) ? _sir_handleerr(ret) : true;
#elif (defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_12_2__)) || \
      (defined(__GLIBC__) && GLIBC_VERSION >= 21200 && defined(_GNU_SOURCE)) || \
       defined(USE_PTHREAD_GETNAME_NP)
    int ret = pthread_setname_np(pthread_self(), name);
    return (0 != ret) ? _sir_handleerr(ret) : true;
#elif defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_11_3__)
    pthread_set_name_np(pthread_self(), name);
    return true;
#else
# if !defined(SUNLINT)
#  pragma message("unable to determine how to set a thread name")
# endif
    SIR_UNUSED(name);
    return false;
#endif
}

bool _sir_gethostname(char name[SIR_MAXHOST]) {
#if !defined(__WIN__)
    int ret = gethostname(name, SIR_MAXHOST - 1);
    return 0 == ret ? true : _sir_handleerr(errno);
#else
    WSADATA wsad = {0};
    int ret      = WSAStartup(MAKEWORD(2, 2), &wsad);
    if (0 != ret)
        return _sir_handlewin32err(ret);

    if (SOCKET_ERROR == gethostname(name, SIR_MAXHOST)) {
        int err = WSAGetLastError();
        WSACleanup();
        return _sir_handlewin32err(err);
    }

    WSACleanup();
    return true;
#endif /* !__WIN__ */
}
