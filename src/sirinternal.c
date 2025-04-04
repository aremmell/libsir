/*
 * sirinternal.c
 *
 * Version: 2.2.6
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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
 *
 * -----------------------------------------------------------------------------
 */

//-V::522
#include "sir/internal.h"
#include "sir/console.h"
#include "sir/defaults.h"
#include "sir/filecache.h"
#include "sir/plugins.h"
#include "sir/textstyle.h"
#include "sir/filesystem.h"
#include "sir/mutex.h"

#if defined(__WIN__)
# if defined(SIR_EVENTLOG_ENABLED)
#  include "sir/wineventlog.h"
#  pragma comment(lib, "advapi32.lib")
# endif
# if defined(__EMBARCADEROC__) && defined(_WIN64)
#  pragma comment(lib, "ws2_32.a")
# else
#  pragma comment(lib, "ws2_32.lib")
# endif
#endif

static sirconfig _sir_cfg      = {0};
static sirfcache _sir_fc       = {0};
static sir_plugincache _sir_pc = {0};

#if !defined(__IMPORTC__)
static sir_mutex cfg_mutex  = SIR_MUTEX_INIT;
static sir_mutex fc_mutex   = SIR_MUTEX_INIT;
static sir_mutex pc_mutex   = SIR_MUTEX_INIT;
# if !defined(SIR_NO_TEXT_STYLING)
static sir_mutex ts_mutex   = SIR_MUTEX_INIT;
# endif
#else
static sir_mutex cfg_mutex  = {0};
static sir_mutex fc_mutex   = {0};
static sir_mutex pc_mutex   = {0};
# if !defined(SIR_NO_TEXT_STYLING)
static sir_mutex ts_mutex   = {0};
# endif
#endif
static sir_once static_once = SIR_ONCE_INIT;

#if defined(__WIN__)
static LARGE_INTEGER _sir_perfcntr_freq = {0};
#endif

#if defined(__HAVE_ATOMIC_H__)
static atomic_uint_fast32_t _sir_magic;
#else
static volatile uint32_t _sir_magic = 0U;
#endif

static _sir_thread_local char _sir_tid[SIR_MAXPID]   = {0};
static _sir_thread_local sir_time _sir_last_thrd_chk = {0};
static _sir_thread_local time_t _sir_last_timestamp  = 0;

bool _sir_makeinit(sirinit* si) {
    bool retval = _sir_validptr(si);

    if (retval) {
        (void)memset(si, 0, sizeof(sirinit));

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

    /* can only fail on Windows. */
    bool once_init = _sir_once(&static_once, _sir_init_static_once);
#if !defined(__WIN__)
    SIR_UNUSED(once_init);
#else
    if (!once_init) {
        _sir_selflog("error: static data initialization routine failed!");
        return false;
    }
#endif

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

    bool init = true;

#if defined(__HAVE_ATOMIC_H__)
    atomic_store(&_sir_magic, _SIR_MAGIC);
#else
    _sir_magic = _SIR_MAGIC;
#endif

    _sir_reset_tls();

#if defined(__WIN__)
    _sir_initialize_stdio();
#else
    tzset();
#endif

#if !defined(SIR_NO_TEXT_STYLING)
    if (!_sir_setcolormode(SIRCM_16)) {
        init = false;
        _sir_selflog("error: failed to set color mode!");
    }

    if (!_sir_resettextstyles()) {
        init = false;
        _sir_selflog("error: failed to reset text styles!");
    }
#endif

    (void)memset(&_cfg->state, 0, sizeof(_cfg->state));
    (void)memcpy(&_cfg->si, si, sizeof(sirinit));

    /* forcibly null-terminate the process name. */
    _cfg->si.name[SIR_MAXNAME - 1] = '\0';

    /* store PID. */
    _cfg->state.pid = _sir_getpid();

    (void)snprintf(_cfg->state.pidbuf, SIR_MAXPID, SIR_PIDFORMAT,
        PID_CAST _cfg->state.pid);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    /* initialize system logger. */
    _sir_syslog_reset(&_cfg->si.d_syslog);

    if (_cfg->si.d_syslog.levels != SIRL_NONE &&
        !_sir_syslog_init(_cfg->si.name, &_cfg->si.d_syslog)) {
        init = false;
        _sir_selflog("failed to initialize system logger!");
    }
#endif

    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    _sir_selflog("initialized %s", (init ? "successfully" : "with errors")); //-V547

    SIR_ASSERT(init);
    return init;
}

bool _sir_cleanup(void) {
    if (!_sir_sanity())
        return false;

    _SIR_LOCK_SECTION(sirfcache, sfc, SIRMI_FILECACHE, false);
    bool cleanup   = true;
    bool destroyfc = _sir_fcache_destroy(sfc);
    SIR_ASSERT(destroyfc);

    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);
    _sir_eqland(cleanup, destroyfc);

#if !defined(SIR_NO_PLUGINS)
    _SIR_LOCK_SECTION(sir_plugincache, spc, SIRMI_PLUGINCACHE, false);
    bool destroypc = _sir_plugin_cache_destroy(spc);
    SIR_ASSERT(destroypc);
    _SIR_UNLOCK_SECTION(SIRMI_PLUGINCACHE);
    _sir_eqland(cleanup, destroypc);
#endif

    _SIR_LOCK_SECTION(sirconfig, _cfg, SIRMI_CONFIG, false);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    if (!_sir_syslog_close(&_cfg->si.d_syslog)) {
        cleanup = false;
        _sir_selflog("error: failed to close system logger!");
    }

    _sir_syslog_reset(&_cfg->si.d_syslog);
#endif

#if !defined(SIR_NO_TEXT_STYLING)
    if (!_sir_resettextstyles()) {
        cleanup = false;
        _sir_selflog("error: failed to reset text styles!");
    }
#endif

#if defined(__HAVE_ATOMIC_H__)
    atomic_store(&_sir_magic, 0);
#else
    _sir_magic = 0U;
#endif

    _sir_reset_tls();

    (void)memset(_cfg, 0, sizeof(sirconfig));
    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    _sir_selflog("cleaned up %s", (cleanup ? "successfully" : "with errors"));

    SIR_ASSERT(cleanup);
    return cleanup;
}

bool _sir_isinitialized(void) {
#if defined(__HAVE_ATOMIC_H__)
    if (_SIR_MAGIC == atomic_load(&_sir_magic))
        return true;
#else
    if (_SIR_MAGIC == _sir_magic)
        return true;
#endif
    return false;
}

bool _sir_sanity(void) {
    if (_sir_isinitialized())
        return true;
    return _sir_seterror(_SIR_E_NOTREADY);
}

bool _sir_init_sanity(const sirinit* si) {
    if (!_sir_validptr(si))
        return false;

    bool levelcheck = true;
    _sir_eqland(levelcheck, _sir_validlevels(si->d_stdout.levels));
    _sir_eqland(levelcheck, _sir_validlevels(si->d_stderr.levels));

    bool regcheck = true;
    _sir_eqland(regcheck, SIRL_NONE == si->d_stdout.levels);
    _sir_eqland(regcheck, SIRL_NONE == si->d_stderr.levels);

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_eqland(levelcheck, _sir_validlevels(si->d_syslog.levels));
    _sir_eqland(regcheck, SIRL_NONE == si->d_syslog.levels);
#endif

    if (regcheck)
        _sir_selflog("warning: no level registrations set!");

    bool optscheck = true;
    _sir_eqland(optscheck, _sir_validopts(si->d_stdout.opts));
    _sir_eqland(optscheck, _sir_validopts(si->d_stderr.opts));

#if !defined(SIR_NO_SYSTEM_LOGGERS)
    _sir_eqland(optscheck, _sir_validopts(si->d_syslog.opts));
#endif

    return levelcheck && optscheck;
}

void _sir_reset_tls(void) {
    _sir_resetstr(_sir_tid);
    (void)memset(&_sir_last_thrd_chk, 0, sizeof(sir_time));
    _sir_last_timestamp = 0;
    _sir_reset_tls_error();
}

static
bool _sir_updatelevels(const char* name, sir_levels* old, const sir_levels* new) {
    bool retval = _sir_validstr(name) && _sir_validptr(old) && _sir_validptr(new);
    if (retval) {
        if (*old != *new) {
            _sir_selflog("updating %s levels from %04"PRIx16" to %04"PRIx16, name, *old, *new);
            *old = *new;
        } else {
            _sir_selflog("skipped superfluous update of %s levels: %04"PRIx16, name, *old);
        }
    }
    return retval;
}

static
bool _sir_updateopts(const char* name, sir_options* old, const sir_options* new) {
    bool retval = _sir_validstr(name) && _sir_validptr(old) && _sir_validptr(new);
    if (retval) {
        if (*old != *new) {
            _sir_selflog("updating %s options from %08"PRIx32" to %08"PRIx32, name, *old, *new);
            *old = *new;
        } else {
            _sir_selflog("skipped superfluous update of %s options: %08"PRIx32, name, *old);
        }
    }
    return retval;
}

bool _sir_stdoutlevels(sirinit* si, const sir_update_config_data* data) {
    return _sir_updatelevels(SIR_DESTNAME_STDOUT, &si->d_stdout.levels, data->levels);
}

bool _sir_stdoutopts(sirinit* si, const sir_update_config_data* data) {
    return _sir_updateopts(SIR_DESTNAME_STDOUT, &si->d_stdout.opts, data->opts);
}

bool _sir_stderrlevels(sirinit* si, const sir_update_config_data* data) {
    return _sir_updatelevels(SIR_DESTNAME_STDERR, &si->d_stderr.levels, data->levels);
}

bool _sir_stderropts(sirinit* si, const sir_update_config_data* data) {
    return _sir_updateopts(SIR_DESTNAME_STDERR, &si->d_stderr.opts, data->opts);
}

bool _sir_sysloglevels(sirinit* si, const sir_update_config_data* data) {
    bool updated = _sir_updatelevels(SIR_DESTNAME_SYSLOG, &si->d_syslog.levels, data->levels);
    if (updated) {
        _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_LEVELS);
        updated = _sir_syslog_updated(si, data);
        _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_LEVELS);
    }
    return updated;
}

bool _sir_syslogopts(sirinit* si, const sir_update_config_data* data) {
    bool updated = _sir_updateopts(SIR_DESTNAME_SYSLOG, &si->d_syslog.opts, data->opts);
    if (updated) {
        _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_OPTIONS);
        updated = _sir_syslog_updated(si, data);
        _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_OPTIONS);
    }
    return updated;
}

bool _sir_syslogid(sirinit* si, const sir_update_config_data* data) {
    bool retval = _sir_validptr(si) && _sir_validptr(data);

    if (retval) {
        bool cur_ok = _sir_validstrnofail(si->d_syslog.identity);
        if (!cur_ok || 0 != strncmp(si->d_syslog.identity, data->sl_identity, SIR_MAX_SYSLOG_ID)) {
            _sir_selflog("updating %s identity from '%s' to '%s'", SIR_DESTNAME_SYSLOG,
                si->d_syslog.identity, data->sl_identity);
            (void)_sir_strncpy(si->d_syslog.identity, SIR_MAX_SYSLOG_ID, data->sl_identity,
                strnlen(data->sl_identity, SIR_MAX_SYSLOG_ID));
            _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_IDENTITY);
            retval = _sir_syslog_updated(si, data);
            _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_IDENTITY);
        } else {
            _sir_selflog("skipped superfluous update of %s identity: '%s'", SIR_DESTNAME_SYSLOG,
                si->d_syslog.identity);
        }
    }

    return retval;
}

bool _sir_syslogcat(sirinit* si, const sir_update_config_data* data) {
    bool retval = _sir_validptr(si) && _sir_validptr(data);

    if (retval) {
        bool cur_ok = _sir_validstrnofail(si->d_syslog.category);
        if (!cur_ok || 0 != strncmp(si->d_syslog.category, data->sl_category, SIR_MAX_SYSLOG_CAT)) {
            _sir_selflog("updating %s category from '%s' to '%s'", SIR_DESTNAME_SYSLOG,
                si->d_syslog.category, data->sl_category);
            (void)_sir_strncpy(si->d_syslog.category, SIR_MAX_SYSLOG_CAT, data->sl_category,
                strnlen(data->sl_category, SIR_MAX_SYSLOG_CAT));
            _sir_setbitshigh(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_CATEGORY);
            retval = _sir_syslog_updated(si, data);
            _sir_setbitslow(&si->d_syslog._state.mask, SIRSL_UPDATED | SIRSL_CATEGORY);
        } else {
            _sir_selflog("skipped superfluous update of %s category: '%s'", SIR_DESTNAME_SYSLOG,
                si->d_syslog.identity);
        }
    }

    return retval;
}

bool _sir_writeinit(const sir_update_config_data* data, sirinit_update update) {
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
            tmpm   = &cfg_mutex;
            tmpsec = &_sir_cfg;
            break;
        case SIRMI_FILECACHE:
            tmpm   = &fc_mutex;
            tmpsec = &_sir_fc;
            break;
        case SIRMI_PLUGINCACHE:
            tmpm   = &pc_mutex;
            tmpsec = &_sir_pc;
            break;
#if !defined(SIR_NO_TEXT_STYLING)
        case SIRMI_TEXTSTYLE:
            tmpm   = &ts_mutex;
            tmpsec = &sir_text_style_section;
            break;
#endif
        default: // GCOVR_EXCL_START
#if !defined(SIR_NO_TEXT_STYLING)
            SIR_ASSERT(false);
#endif
            tmpm   = NULL;
            tmpsec = NULL;
            break;
    } // GCOVR_EXCL_STOP

    *m = tmpm;

    if (section)
        *section = tmpsec;

    return *m != NULL && (!section || *section != NULL);
}

#if !defined(__WIN__)
void _sir_init_static_once(void) {
    (void)_sir_init_common_static();
}
#else /* __WIN__ */
BOOL CALLBACK _sir_init_static_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);
    return _sir_init_common_static() ? TRUE : FALSE;
}
#endif

bool _sir_init_common_static(void) {
#if defined(__HAVE_ATOMIC_H__)
    atomic_init(&_sir_magic, 0);
#endif

#if defined(__WIN__)
    (void)QueryPerformanceFrequency(&_sir_perfcntr_freq);
#endif

    bool created = _sir_mutexcreate(&cfg_mutex);
    SIR_ASSERT(created);

    _sir_eqland(created, _sir_mutexcreate(&fc_mutex));
    SIR_ASSERT(created);

    _sir_eqland(created, _sir_mutexcreate(&pc_mutex));
    SIR_ASSERT(created);

#if !defined(SIR_NO_TEXT_STYLING)
    _sir_eqland(created, _sir_mutexcreate(&ts_mutex));
    SIR_ASSERT(created);
#endif

    return created;
}

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
        (now_sec - _cfg->state.last_hname_chk) > SIR_HNAME_CHK_INTERVAL) {
        _sir_selflog("updating hostname...");
        if (!_sir_gethostname(_cfg->state.hostname)) {
            _sir_selflog("error: failed to get hostname!");
        } else {
            _cfg->state.last_hname_chk = now_sec;
            _sir_selflog("hostname: '%s'", _cfg->state.hostname);
        }
    }

    /* format timestamp (h/m/s only if the integer time has changed). */
    long now_msec = 0L;
    bool gettime = _sir_clock_gettime(SIR_WALLCLOCK, &now_sec, &now_msec);
    SIR_ASSERT_UNUSED(gettime, gettime);

    /* milliseconds. */
    _sir_snprintf_trunc(buf.msec, SIR_MAXMSEC, SIR_MSECFORMAT, now_msec);

    /* hours/minutes/seconds. */
    if (now_sec > _sir_last_timestamp || !*_cfg->state.timestamp) {
        _sir_last_timestamp = now_sec;
        bool fmt = _sir_formattime(now_sec, _cfg->state.timestamp, SIR_TIMEFORMAT);
        SIR_ASSERT_UNUSED(fmt, fmt);
    }

    /* check elapsed time since updating thread identifier/name. */
    sir_time thrd_chk;
    double msec_since_thrd_chk = _sir_msec_since(&_sir_last_thrd_chk, &thrd_chk);

    /* update the thread identifier/name if enough time has elapsed. */
    if (msec_since_thrd_chk > SIR_THRD_CHK_INTERVAL) {
        _sir_last_thrd_chk = thrd_chk;

        pid_t tid         = _sir_gettid();
        bool resolved_tid = false;

        /* prefer thread names. */
        resolved_tid = _sir_getthreadname(_sir_tid);

        /* if tid is identical to pid... */
        if (!resolved_tid && tid == _cfg->state.pid) {
            /* don't use anything to identify the thread. */
            _sir_resetstr(_sir_tid);
            resolved_tid = true;
        }

        /* fall back on tid. */
        if (!resolved_tid)
            _sir_snprintf_trunc(_sir_tid, SIR_MAXPID, SIR_TIDFORMAT,
                PID_CAST tid);
    }

    sirconfig cfg;
    (void)memcpy(&cfg, _cfg, sizeof(sirconfig));
    _SIR_UNLOCK_SECTION(SIRMI_CONFIG);

    buf.timestamp = cfg.state.timestamp;
    buf.hostname  = cfg.state.hostname;
    buf.pid       = cfg.state.pidbuf;
    buf.name      = cfg.si.name;

#if !defined(SIR_NO_TEXT_STYLING)
    const char* style_str = _sir_gettextstyle(level);

    SIR_ASSERT(NULL != style_str);
    if (NULL != style_str)
        (void)_sir_strncpy(buf.style, SIR_MAXSTYLE, style_str,
            strnlen(style_str, SIR_MAXSTYLE));
#endif

    buf.level = _sir_formattedlevelstr(level);

    if (_sir_validstrnofail(_sir_tid))
        (void)_sir_strncpy(buf.tid, SIR_MAXPID, _sir_tid,
            strnlen(_sir_tid, SIR_MAXPID));

    (void)vsnprintf(buf.message, SIR_MAXMESSAGE, format, args);

    if (!_sir_validstrnofail(buf.message))
        return _sir_seterror(_SIR_E_INTERNAL);

    bool match             = false;
    bool exit_early        = false;
    bool update_last_props = true;
    uint64_t hash          = 0ULL;

    if (cfg.state.last.level == level &&
        cfg.state.last.prefix[0] == buf.message[0]  &&
        cfg.state.last.prefix[1] == buf.message[1]) {
        hash  = FNV64_1a(buf.message);
        match = cfg.state.last.hash == hash;
    }

    if (match) {
        cfg.state.last.counter++;

        if (cfg.state.last.counter >= cfg.state.last.threshold - 2) {
            size_t old_threshold = cfg.state.last.threshold;

            update_last_props = false;
            cfg.state.last.threshold *= SIR_SQUELCH_BACKOFF_FACTOR;
            cfg.state.last.squelch = true;

            _sir_selflog("hit squelch threshold of %zu; setting new threshold"
                         " to %zu (factor: %d)", old_threshold,
                         cfg.state.last.threshold, SIR_SQUELCH_BACKOFF_FACTOR);

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
        _cfg->state.last.level     = level;
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

bool _sir_dispatch(const sirinit* si, sir_level level, sirbuf* buf) {
    bool retval       = true;
    size_t dispatched = 0;
    size_t wanted     = 0;

#if !defined(SIR_NO_TEXT_STYLING)
    static const bool styling = true;
#else
    static const bool styling = false;
#endif

    if (_sir_bittest(si->d_stdout.levels, level)) {
        const char* writef = _sir_format(styling, si->d_stdout.opts, buf);
        bool wrote         = _sir_validstrnofail(writef) &&
            _sir_write_stdout(writef, buf->output_len);
        _sir_eqland(retval, wrote);

        if (wrote)
            dispatched++;
        wanted++;
    }

    if (_sir_bittest(si->d_stderr.levels, level)) {
        const char* writef = _sir_format(styling, si->d_stderr.opts, buf);
        bool wrote         = _sir_validstrnofail(writef) &&
            _sir_write_stderr(writef, buf->output_len);
        _sir_eqland(retval, wrote);

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

    _SIR_LOCK_SECTION(const sirfcache, sfc, SIRMI_FILECACHE, false);
    size_t fdispatched = 0;
    size_t fwanted     = 0;
    _sir_eqland(retval, _sir_fcache_dispatch(sfc, level, buf, &fdispatched, &fwanted));
    _SIR_UNLOCK_SECTION(SIRMI_FILECACHE);

    dispatched += fdispatched;
    wanted += fwanted;

#if !defined(SIR_NO_PLUGINS)
    _SIR_LOCK_SECTION(const sir_plugincache, spc, SIRMI_PLUGINCACHE, false);
    size_t pdispatched = 0;
    size_t pwanted     = 0;
    _sir_eqland(retval, _sir_plugin_cache_dispatch(spc, level, buf, &pdispatched, &pwanted));
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
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->style, SIR_MAXSTYLE);

        if (!_sir_bittest(opts, SIRO_NOTIME)) {
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->timestamp, SIR_MAXTIME);
            first = false;

#if defined(SIR_MSEC_TIMER)
            if (!_sir_bittest(opts, SIRO_NOMSEC))
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->msec, SIR_MAXMSEC);
#endif
        }

        if (!_sir_bittest(opts, SIRO_NOHOST) && _sir_validstrnofail(buf->hostname)) {
            if (!first)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->hostname, SIR_MAXHOST);
            first = false;
        }

        if (!_sir_bittest(opts, SIRO_NOLEVEL)) {
            if (!first)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->level, SIR_MAXLEVEL);
            first = false;
        }

        bool name = false;
        if (!_sir_bittest(opts, SIRO_NONAME) && _sir_validstrnofail(buf->name)) {
            if (!first)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->name, SIR_MAXNAME);
            first = false;
            name  = true;
        }

        bool wantpid = !_sir_bittest(opts, SIRO_NOPID) && _sir_validstrnofail(buf->pid);
        bool wanttid = !_sir_bittest(opts, SIRO_NOTID) && _sir_validstrnofail(buf->tid);

        if (wantpid || wanttid) {
            if (name)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_PIDPREFIX, 1);
            else if (!first)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, " ", 1);

            if (wantpid)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->pid, SIR_MAXPID);

            if (wanttid) {
                if (wantpid)
                    (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_PIDSEPARATOR, 1);
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->tid, SIR_MAXPID);
            }

            if (name)
                (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_PIDSUFFIX, 1);

            first = false;
        }

        if (!first)
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, ": ", 2);

        (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, buf->message, SIR_MAXMESSAGE);

        if (styling)
            (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_ESC_RST, SIR_MAXSTYLE);

#if defined(SIR_USE_EOL_CRLF)
        (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_EOL, 2);
#else
        (void)_sir_strncat(buf->output, SIR_MAXOUTPUT, SIR_EOL, 1);
#endif

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
            (void)_sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, name, strnlen(name, SIR_MAX_SYSLOG_ID));
        } else {
            _sir_selflog("name is no good; trying filename");
            char* appbasename = _sir_getappbasename();
            if (_sir_validstrnofail(appbasename)) {
                _sir_selflog("filename is good: %s", appbasename);
                (void)_sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, appbasename,
                    strnlen(appbasename, SIR_MAX_SYSLOG_ID));
            } else {
                _sir_selflog("filename no good; using fallback");
                (void)_sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, SIR_FALLBACK_SYSLOG_ID,
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
        (void)_sir_strncpy(ctx->category, SIR_MAX_SYSLOG_CAT, SIR_FALLBACK_SYSLOG_CAT,
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
# elif defined(SIR_EVENTLOG_ENABLED)
    DWORD reg = EventRegister(&SIR_EVENTLOG_GUID, NULL, NULL,
        (PREGHANDLE)&ctx->_state.logger);
    if (ERROR_SUCCESS == reg) {
        _sir_selflog("opened eventlog('%s')", ctx->identity);
    } else {
        /* not fatal; logging calls will just silently fail. */
        _sir_selflog("failed to open eventlog! error: %lu", reg);
        (void)_sir_handlewin32err(reg);
    }
# endif

    _sir_setbitshigh(&ctx->_state.mask, SIRSL_IS_OPEN);
    return true;
#else
    SIR_UNUSED(ctx);
    return false;
#endif
}

bool _sir_syslog_write(sir_level level, const sirbuf* buf, const sir_syslog_dest* ctx) {
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
    else if (SIRL_CRIT == level || SIRL_ALERT == level || SIRL_EMERG == level)
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
        default: /* this should never happen. */
            SIR_ASSERT(false);
            syslog_level = LOG_DEBUG;
        // GCOVR_EXCL_STOP
    }

    syslog(syslog_level, "%s", buf->message);
    return true;
# elif defined(SIR_EVENTLOG_ENABLED)
    const EVENT_DESCRIPTOR* edesc = NULL;
    if (SIRL_DEBUG == level)
        edesc = &SIR_EVT_DEBUG;
    else if (SIRL_INFO == level || SIRL_NOTICE == level)
        edesc = &SIR_EVT_INFO;
    else if (SIRL_WARN == level)
        edesc = &SIR_EVT_WARNING;
    else if (SIRL_ERROR == level)
        edesc = &SIR_EVT_ERROR;
    else if (SIRL_CRIT == level || SIRL_ALERT == level || SIRL_EMERG == level)
        edesc = &SIR_EVT_CRITICAL;

    SIR_ASSERT(NULL != edesc);
    if (NULL == edesc)
        return _sir_seterror(_SIR_E_INTERNAL);

#  if defined(__HAVE_STDC_SECURE_OR_EXT1__)
    size_t msg_len = strnlen_s(buf->message, SIR_MAXMESSAGE) + 1;
#  else
    size_t msg_len = strnlen(buf->message, SIR_MAXMESSAGE) + 1;
#  endif
    int wlen = MultiByteToWideChar(CP_UTF8, 0UL, buf->message, (int)msg_len, NULL, 0);
    if (wlen <= 0)
        return _sir_handlewin32err(GetLastError());

    DWORD write = 1UL;
    wchar_t* wmsg = calloc(wlen, sizeof(wchar_t));
    if (NULL != wmsg) {
        int conv = MultiByteToWideChar(CP_UTF8, 0UL, buf->message, (int)msg_len, wmsg, wlen);
        if (conv > 0) {
            EVENT_DATA_DESCRIPTOR eddesc = {0};
            EventDataDescCreate(&eddesc, wmsg, (ULONG)(wlen * sizeof(wchar_t)));

            write = EventWrite((REGHANDLE)ctx->_state.logger, edesc, 1UL, &eddesc);
            if (ERROR_SUCCESS != write) {
                _sir_selflog("failed to write eventlog! error: %lu", write);
                (void)_sir_handlewin32err(write);
            }
        }
        _sir_safefree(&wmsg);
    }

    return ERROR_SUCCESS == write;
# else
    SIR_UNUSED(level);
    SIR_UNUSED(buf);
    SIR_UNUSED(ctx);
    return false;
# endif
#else
    SIR_UNUSED(level);
    SIR_UNUSED(buf);
    SIR_UNUSED(ctx);
    return false;
#endif
}

bool _sir_syslog_updated(sirinit* si, const sir_update_config_data* data) {
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
        /* for os_log, if initialized and open already, only need to reconfigure
         * if identity or category changed. */
        must_init = (!is_init || !is_open) || (identity || category);
# elif defined(SIR_SYSLOG_ENABLED)
        /* for syslog, if initialized and open already, only need to reconfigure
         * if identity or options changed. */
        must_init = (!is_init || !is_open) || (identity || options);
# elif defined(SIR_EVENTLOG_ENABLED)
        /* for event log, if initialized and open already, only need to reconfigure
         * if identity changed. */
        must_init = (!is_init || !is_open) || identity;
# endif
        bool init = true;
        if (must_init) {
            if (is_open) /* close first; open will fail otherwise. */
                init = _sir_syslog_close(&si->d_syslog);

            _sir_selflog("re-init...");
            _sir_eqland(init, _sir_syslog_init(si->name, &si->d_syslog));
            _sir_selflog("re-init %s", init ? "succeeded" : "failed");
        } else {
            _sir_selflog("no re-init necessary");
        }

        return init;
    }

    return false;
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
    /* Evidently, you don't need to close the handle returned from os_log_create(), and
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
# elif defined(SIR_EVENTLOG_ENABLED)
    ULONG unreg = EventUnregister((REGHANDLE)ctx->_state.logger);
    _sir_setbitslow(&ctx->_state.mask, SIRSL_IS_OPEN);
    if (ERROR_SUCCESS == unreg)
        _sir_selflog("closed log");
    else
        _sir_selflog("error: failed to close log");

    return ERROR_SUCCESS == unreg;
# else
    SIR_UNUSED(ctx);
    return false;
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
                *msecbuf = ts.tv_nsec / 1000000L;
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
        SIR_UNUSED(clock);
        (void)time(tbuf);
        if (msecbuf)
            *msecbuf = 0L;
#endif
        return true;
    }
    return false;
}

double _sir_msec_since(const sir_time* when, sir_time* out) {
    if (!_sir_validptr(out))
        return 0.0;
#if !defined(__WIN__)
    out->sec = 0;
    out->msec = 0L;

    bool gettime = _sir_clock_gettime(SIR_INTERVALCLOCK, &out->sec, &out->msec);
    SIR_ASSERT(gettime);

    if (!_sir_validptrnofail(when) || !gettime || (out->sec < when->sec ||
        (out->sec == when->sec && out->msec < when->msec)))
        return 0.0;

    return ((((double)out->sec) * 1e3) + (double)out->msec) -
           ((((double)when->sec) * 1e3) + (double)when->msec);
#else /* __WIN__ */
    SIR_ASSERT(_sir_perfcntr_freq.QuadPart > 0LL);

    if (_sir_perfcntr_freq.QuadPart <= 0LL)
        (void)QueryPerformanceFrequency(&_sir_perfcntr_freq);

    (void)QueryPerformanceCounter(&out->counter);

    if (!_sir_validptrnofail(when) || out->counter.QuadPart <= when->counter.QuadPart)
        return 0.0;

    double msec_ratio = ((double)_sir_perfcntr_freq.QuadPart) / 1e3;
    return ((double)(out->counter.QuadPart - when->counter.QuadPart)) / msec_ratio;
#endif
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
      defined(__DragonFly__) || defined(__CYGWIN__) || defined(_AIX) || \
      defined(__EMSCRIPTEN__) || defined(__QNX__)
# if defined(__CYGWIN__)
    tid = (pid_t)(uintptr_t)pthread_self();
# else
    tid = (pid_t)pthread_self();
# endif
#elif defined(__HAIKU__)
    tid = get_pthread_thread_id(pthread_self());
#elif defined(__linux__) || defined(__serenity__)
# if (defined(__GLIBC__) && GLIBC_VERSION >= 23000) || defined(__serenity__)
    tid = gettid();
# else
    tid = syscall(SYS_gettid);
# endif
#elif defined(__WIN__)
    tid = (pid_t)GetCurrentThreadId();
#else
# error "unable to determine how to get a thread identifier"
#endif
    return tid;
}

bool _sir_getthreadname(char name[SIR_MAXPID]) {
    _sir_resetstr(name);
#if defined(__MACOS__) || (defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_12_2__)) || \
    (defined(__linux__) && defined(__UCLIBC__) && defined(_GNU_SOURCE)) || \
    (defined(__GLIBC__) && GLIBC_VERSION >= 21200 && defined(_GNU_SOURCE)) || \
    (defined(__ANDROID__) &&  __ANDROID_API__ >= 26) || defined(SIR_PTHREAD_GETNAME_NP) || \
    defined(__serenity__) || (defined(__linux__) && !defined(__GLIBC__) && \
    defined(_GNU_SOURCE) && defined(__NEED_pthread_t)) || defined(__QNX__)
    int ret = pthread_getname_np(pthread_self(), name, SIR_MAXPID);
    if (0 != ret)
        return _sir_handleerr(ret);
# if defined(__HAIKU__)
    if (!(strncmp(name, "pthread func", SIR_MAXPID)))
        (void)snprintf(name, SIR_MAXPID, "%ld", (long)get_pthread_thread_id(pthread_self()));
# endif
    return _sir_validstrnofail(name);
#elif defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_11_3__)
    pthread_get_name_np(pthread_self(), name, SIR_MAXPID);
    return _sir_validstrnofail(name);
#elif defined(__WIN__)
    wchar_t* wname = NULL;
    HRESULT hr     = GetThreadDescription(GetCurrentThread(), &wname);
    if (FAILED(hr))
        return _sir_handlewin32err(GetLastError());
    bool success = true;
# if defined(__HAVE_STDC_SECURE_OR_EXT1__)
    size_t wlen = wcsnlen_s(wname, SIR_MAXPID);
# elif defined(__EMBARCADEROC__) && (__clang_major__ < 15)
    size_t wlen = wcslen(wname);
# else
    size_t wlen = wcsnlen(wname, SIR_MAXPID);
# endif
    if (wlen > 0) {
        if (!WideCharToMultiByte(CP_UTF8, 0UL, wname, (int)wlen, name,
            SIR_MAXPID, NULL, NULL)) {
            success = false;
            (void)_sir_handlewin32err(GetLastError());
        }
    }
    (void)LocalFree(wname);
    return success && _sir_validstrnofail(name);
#else
# if !defined(SUNLINT) && !defined(_AIX) && !defined(__HURD__)
#  pragma message("unable to determine how to get a thread name")
# endif
    SIR_UNUSED(name);
    return false;
#endif
}

bool _sir_setthreadname(const char* name) {
    if (!_sir_validptr(name))
        return false;
#if defined(__MACOS__)
    int ret = pthread_setname_np(name);
    return (0 != ret) ? _sir_handleerr(ret) : true;
#elif defined(__HAIKU__)
    status_t ret = rename_thread(find_thread(NULL), name);
    return (B_OK != ret) ? _sir_handleerr((int)ret) : true;
#elif defined(__NetBSD__)
    int ret = pthread_setname_np(pthread_self(), "%s", name);
    return (0 != ret) ? _sir_handleerr(ret) : true;
#elif (defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_12_2__)) || \
      (defined(__linux__) && defined(__UCLIBC__) && defined(_GNU_SOURCE)) || \
      (defined(__GLIBC__) && GLIBC_VERSION >= 21200 && defined(_GNU_SOURCE)) || \
       defined(__QNXNTO__) || defined(__SOLARIS__) || defined(SIR_PTHREAD_GETNAME_NP) || \
       defined(__ANDROID__) && !defined(__OpenBSD__) || defined(__serenity__) || \
      (defined(__linux__) && !defined(__GLIBC__) && \
       defined(_GNU_SOURCE) && defined(__NEED_pthread_t))
    int ret = pthread_setname_np(pthread_self(), name);
    return (0 != ret) ? _sir_handleerr(ret) : true;
#elif defined(__OpenBSD__) || defined(__BSD__) && defined(__FreeBSD_PTHREAD_NP_11_3__)
    pthread_set_name_np(pthread_self(), name);
    return true;
#elif defined(__WIN__)
# if defined(__HAVE_STDC_SECURE_OR_EXT1__)
    size_t name_len = strnlen_s(name, SIR_MAXPID);
# else
    size_t name_len = strnlen(name, SIR_MAXPID);
# endif
    if (0 == name_len)
        name_len = 1;

    wchar_t buf[SIR_MAXPID] = {0};
    if (!MultiByteToWideChar(CP_UTF8, 0UL, name, (int)name_len, buf, SIR_MAXPID))
        return _sir_handlewin32err(GetLastError());

    HRESULT hr = SetThreadDescription(GetCurrentThread(), buf);
    return FAILED(hr) ? _sir_handlewin32err(hr) : true;
#else
# if !defined(SUNLINT) && !defined(_AIX)
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

long __sir_nprocs(bool test_mode) {
    long nprocs = 0;

#if defined(_AIX)
    nprocs = (long)_system_configuration.ncpus;
    _sir_selflog("AIX _system_configuration.ncpus reports %ld processor(s)", nprocs);
#endif

#if defined(__WIN__)
    SYSTEM_INFO system_info;
    ZeroMemory(&system_info, sizeof(system_info));
    GetSystemInfo(&system_info);
    nprocs = (long)system_info.dwNumberOfProcessors;
    _sir_selflog("Windows GetSystemInfo() reports %ld processor(s)", nprocs);
#endif

#if defined(__HAIKU__)
    system_info hinfo;
    get_system_info(&hinfo);
    nprocs = (long)hinfo.cpu_count;
    _sir_selflog("Haiku get_system_info() reports %ld processor(s)", nprocs);
#endif

#if defined(SC_NPROCESSORS_ONLN)
# define SIR_SC_NPROCESSORS SC_NPROCESSORS_ONLN
#elif defined(_SC_NPROCESSORS_ONLN)
# define SIR_SC_NPROCESSORS _SC_NPROCESSORS_ONLN
#endif
#if defined(SIR_SC_NPROCESSORS)
    long tprocs = sysconf(SIR_SC_NPROCESSORS);
    _sir_selflog("sysconf() reports %ld processor(s)", tprocs);
    if (tprocs > nprocs)
        nprocs = tprocs;
#endif

#if defined(__linux__) && defined(CPU_COUNT) && !defined(__ANDROID__) && !defined(__UCLIBC__)
    long ctprocs;
    cpu_set_t p_aff;
    memset(&p_aff, 0, sizeof(p_aff));
    if (sched_getaffinity(0, sizeof(p_aff), &p_aff)) {
        ctprocs = 0;
    } else {
        ctprocs = CPU_COUNT(&p_aff);
        _sir_selflog("sched_getaffinity() reports %ld processor(s)", ctprocs);
    }
    if (ctprocs > nprocs)
        nprocs = ctprocs;
#endif

#if defined(CTL_HW) && defined(HW_AVAILCPU)
    int ntprocs = 0;
    size_t sntprocs = sizeof(ntprocs);
    if (sysctl ((int[2]) {CTL_HW, HW_AVAILCPU}, 2, &ntprocs, &sntprocs, NULL, 0)) {
        ntprocs = 0;
    } else {
        _sir_selflog("sysctl(CTL_HW, HW_AVAILCPU) reports %d processor(s)", ntprocs);
        if (ntprocs > nprocs)
            nprocs = (long)ntprocs;
    }
#elif defined(CTL_HW) && defined(HW_NCPU)
    int ntprocs = 0;
    size_t sntprocs = sizeof(ntprocs);
    if (sysctl ((int[2]) {CTL_HW, HW_NCPU}, 2, &ntprocs, &sntprocs, NULL, 0)) {
        ntprocs = 0;
    } else {
        _sir_selflog("sysctl(CTL_HW, HW_NCPU) reports %d processor(s)", ntprocs);
        if (ntprocs > nprocs)
            nprocs = (long)ntprocs;
    }
#elif defined(CTL_HW) && defined(HW_NCPUFOUND)
    int ntprocs = 0;
    size_t sntprocs = sizeof(ntprocs);
    if (sysctl ((int[2]) {CTL_HW, HW_NCPUFOUND}, 2, &ntprocs, &sntprocs, NULL, 0)) {
        ntprocs = 0;
    } else {
        _sir_selflog("sysctl(CTL_HW, HW_NCPUFOUND) reports %d processor(s)", ntprocs);
        if (ntprocs > nprocs)
            nprocs = (long)ntprocs;
    }
#endif

#if defined(__MACOS__)
    int antprocs = 0;
    size_t asntprocs = sizeof(antprocs);
    if (sysctlbyname("hw.ncpu", &antprocs, &asntprocs, NULL, 0)) {
        antprocs = 0;
    } else {
        _sir_selflog("sysctlbyname(hw.ncpu) reports %d processor(s)", antprocs);
        if (antprocs > nprocs)
            nprocs = (long)antprocs;
    }
#endif

#if defined(__QNX__) || defined(__QNXNTO__)
    long qtprocs = (long)_syspage_ptr->num_cpu;
    _sir_selflog("QNX _syspage_ptr->num_cpu reports %ld processor(s)", qtprocs);
    if (qtprocs > nprocs)
        nprocs = qtprocs;
#endif

#if defined(__VXWORKS__)
# if defined(_WRS_CONFIG_SMP)
    long vtprocs = 0;
    cpuset_t vset = vxCpuEnabledGet();
    for (int count = 0; count < 512 && !CPUSET_ISZERO(vset); ++count) {
        if (CPUSET_ISSET(vset, count)) {
            CPUSET_CLR(vset, count);
            vtprocs++;
        }
    }
    _sir_selflog("VxWorks vxCpuEnabledGet() reports %ld processor(s)", vtprocs);
# else
    long vtprocs = 1;
    _sir_selflog("Uniprocessor system or VxWorks SMP is not enabled");
# endif
    if (vtprocs > nprocs)
        nprocs = vtprocs;
#endif

    if (nprocs < 1) {
        _sir_selflog(SIR_BRED("Failed to determine processor count!"));
        if (!test_mode)
            nprocs = 1;
    }

    _sir_selflog("Detected %ld processor(s)", nprocs);
    return nprocs;
}
