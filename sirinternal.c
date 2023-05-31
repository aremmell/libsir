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

        /* initialize system logger. */
        _sir_syslog_reset(&si->d_syslog.levels);

        if (_si->d_syslog.levels != SIRL_NONE) {
            bool opened = _sir_syslog_init(_si->processName, &_si->d_syslog);
#if !defined(SIR_NO_SYSTEM_LOGGERS)            
#pragma message("TODO: what now? Can't return false because of this. I guess just selflog a warning") 
#else
            _SIR_UNUSED(opened);
#endif           
        }

        bool unlock = _sir_unlocksection(_SIRM_INIT);
        assert(unlock);
        return true;
    }

    return false;
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
        bool closed = _sir_syslog_close(&si->d_syslog);
#if !defined(SIR_NO_SYSTEM_LOGGERS)
#pragma message("TODO: what now? Can't return false because of this. I guess just selflog a warning")
        if (!closed) {
            // ...
        }

        _sir_syslog_reset(&si->d_syslog);
#else
        _SIR_UNUSED(closed);
#endif

        _sir_resettextstyles();

#if !defined(_WIN32)
    atomic_store(&_sir_magic, 0);
#else
    _sir_magic = 0;
#endif
        memset(si, 0, sizeof(sirinit));
        cleanup &= _sir_unlocksection(_SIRM_INIT);
    }

    _sir_selflog("cleanup: %s", (cleanup ? "successful" : "with issues"));

    assert(cleanup);
    return cleanup;
}

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

static bool _sir_updatelevels(const char* name, sir_levels* old, sir_levels* new) {
    if (*old != *new) {
        _sir_selflog("updating %s levels from %04x to %04x", name, *old, *new);
        *old = *new;
    } else {
        _sir_selflog("skipped superfluous update of %s levels: %04x", name, *old);
    }
    return true;
}

static bool _sir_updateopts(const char* name, sir_options* old, sir_options* new) {
    if (*old != *new) {
        _sir_selflog("updating %s options from %08x to %08x", name, *old, *new);
        *old = *new;
    } else {
        _sir_selflog("skipped superfluous update of %s options: %08x", name, *old);
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
    return _sir_updatelevels(SIR_DESTNAME_SYSLOG, &si->d_syslog.levels, data->levels);
}

bool _sir_syslogid(sirinit* si, sir_update_config_data* data) {
#pragma message("TODO: see if it's already the same, and return false if so.")    
    int update = _sir_strncpy(si->d_syslog.identity, SIR_MAX_SYSLOG_ID,
        data->sl_identity, strnlen(data->sl_identity, SIR_MAX_SYSLOG_ID));
    _sir_selflog("updated %s identity: '%s'", SIR_DESTNAME_SYSLOG, data->sl_identity);
    return _sir_syslog_updated(si, data);
}

bool _sir_syslogcat(sirinit* si, sir_update_config_data* data) {
    int update = _sir_strncpy(si->d_syslog.category, SIR_MAX_SYSLOG_CAT,
        data->sl_category, strnlen(data->sl_category, SIR_MAX_SYSLOG_CAT));
    _sir_selflog("updated %s category: '%s'", SIR_DESTNAME_SYSLOG, data->sl_category);
    return _sir_syslog_updated(si, data);
}

bool _sir_writeinit(sir_update_config_data* data, sirinit_update update) {

    _sir_seterror(_SIR_E_NOERROR);

    if (_sir_sanity() && _sir_validupdatedata(data) && _sir_notnull(update)) {
        sirinit* si = _sir_locksection(_SIRM_INIT);
        assert(si);

        if (_sir_validptr(si)) {
            bool updated = update(si, data);
            return _sir_unlocksection(_SIRM_INIT) && updated;
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

#if !defined(_WIN32)
void _sir_initialize_once(void) {
    atomic_init(&_sir_magic, 0);
}

void _sir_initmutex_si_once(void) {
    bool unused = _sirmutex_create(&si_mutex);
    _SIR_UNUSED(unused);
}

void _sir_initmutex_fc_once(void) {
    bool unused = _sirmutex_create(&fc_mutex);
    _SIR_UNUSED(unused);
}

void _sir_initmutex_ts_once(void) {
    bool unused = _sirmutex_create(&ts_mutex);
    _SIR_UNUSED(unused);
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
        _sir_selflog("no destinations registered for level %04x", level);
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

bool _sir_syslog_init(const char *name, sir_syslog_dest *ctx) {
    if (!_sir_validptr(name) || !_sir_validptr(ctx))
        return false;

    // Begin resolve identity.
    if (!_sir_validstrnofail(ctx->identity)) {
        _sir_selflog("ctx->identity is no good; trying name");
        if (_sir_validstrnofail(name)) {
            _sir_selflog("using name");
            // name can be used.
            _sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, name,
                strnlen(name, SIR_MAX_SYSLOG_ID));
        } else {
            // try using the process nam
            _sir_selflog("name is no good; trying filename");
            char appfilename[SIR_MAXPATH] = {0};
            if (_sir_getappfilename(appfilename, SIR_MAXPATH)) {
            _sir_selflog("filename is good");
#pragma message("TODO: verify file name returned; make sure it's not an entire path, because it could be.")
                _sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, appfilename,
                    strnlen(appfilename, SIR_MAX_SYSLOG_ID));
            } else {
                // retrieving the process name failed. use fallback.
                _sir_selflog("filename no good; using fallback");
                _sir_strncpy(ctx->identity, SIR_MAX_SYSLOG_ID, SIR_FALLBACK_SYSLOG_ID, 
                    strnlen(SIR_FALLBACK_SYSLOG_ID, SIR_MAX_SYSLOG_ID));
            }            
        }
    } else {
        _sir_selflog("ctx->id is set");
    }

    // category
    if (!_sir_validstrnofail(ctx->category)) {
        _sir_selflog("cat not set; using fallback");
        _sir_strncpy(ctx->category, SIR_MAX_SYSLOG_CAT, SIR_FALLBACK_SYSLOG_CAT,
            strnlen(SIR_FALLBACK_SYSLOG_CAT, SIR_MAX_SYSLOG_CAT));
    } else {
        _sir_selflog("ctx->cat is set");
    }

    bool init = _sir_validstrnofail(ctx->identity) && _sir_validstrnofail(ctx->category);
    if (init) {
        ctx->_state.mask |= SIRSL_IS_INIT;
        _sir_selflog("resolved strings (id: '%s', cat: '%s')", ctx->identity, ctx->category);
    } else {
        _sir_selflog("failed to resolve id|cat; cannot log messages!");
    }

    return init ? _sir_syslog_open(name, ctx) : false;
}

bool _sir_syslog_open(const char *name, sir_syslog_dest *ctx) {
    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_INIT)) {
        _sir_seterror(_SIR_E_INVALID);
        _sir_selflog("not initialized; ignoring");
        return false;
    }
#pragma message("TODO: here, check the mask for modified and re-init if necessary")        
    if (_sir_bittest(ctx->_state.mask, SIRSL_IS_OPEN)) {
        _sir_selflog("already opened log; ignoring");
        return false;
    }

#if defined(SIR_OS_LOG_ENABLED)
    ctx->_state.logger = (void*)os_log_create(ctx->identity, ctx->category);
    ctx->_state.mask |= SIRSL_IS_OPEN;
    _sir_selflog("opened os_log ('%s', '%s')", ctx->identity, ctx->category);
    return true;
#elif defined(SIR_SYSLOG_ENABLED)
    int logopt = LOG_NDELAY | (_sir_bittest(ctx->opts, SIRO_NOPID) ? 0 : LOG_PID);
    int facility = LOG_USER;

    openlog(identity, logopt, facility);
    ctx->_state.mask |= SIRSL_IS_OPEN;
    _sir_selflog("opened syslog('%s', %x, %x)", ctx->identity, logopt, facility);
    return true;
#endif
}

bool _sir_syslog_write(sir_level level, const sirbuf *buf, sir_syslog_dest *ctx) {
    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_INIT)) {
        _sir_seterror(_SIR_E_INVALID);
        _sir_selflog("not initialized; ignoring");
        return false;
    }

    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_OPEN)) {
        _sir_seterror(_SIR_E_INVALID);
        _sir_selflog("log not open; ignoring");
        return false;
    }

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

    os_log((os_log_t)ctx->_state.logger, "%s", buf->message);
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

    if (!_sir_bittest(ctx->_state.mask, SIRSL_IS_OPEN)) {
        _sir_selflog("log not open; ignoring");
        return false;
    }

#if defined(SIR_OS_LOG_ENABLED)
    /* evidently, you don't need to close the handle returned from os_log_create(), and
     * if you make that call again, you'll get the same cached value. so let's keep the
     * value we've got in the global context. */
    ctx->_state.mask &= ~SIRSL_IS_OPEN;
    _sir_selflog("log closure not required");
    return true;
#elif defined(SIR_SYSLOG_ENABLED)
    closelog();
    ctx->_state.mask &= ~SIRSL_IS_OPEN;
    _sir_selflog("closed log");
    return true;
#endif
}

bool _sir_syslog_updated(sirinit* si, sir_update_config_data* data) {
#pragma message("TODO: handle changes and re-open the log if neccessary.")
    // The global config section is locked when this is called, so we're safe
    // to tinker with the log. No other thread could enter a block of code
    // that calls open/close/write while we hold the mutex.
#if defined(SIR_OS_LOG_ENABLED)

    return true;
#elif defined(SIR_SYSLOG_ENABLED)

    return true;
#endif    
}

void _sir_syslog_reset(sir_syslog_dest* ctx) {
    if (_sir_validptr(ctx)) {
        ctx->_state.mask = 0;
        ctx->_state.logger = NULL;
        _sir_selflog("reset state");
    }
}

#else // SIR_NO_SYSTEM_LOGGERS
bool _sir_syslog_init(const char *name, sir_syslog_dest *ctx) {
    _SIR_UNUSED(name);
    _SIR_UNUSED(ctx);
    return false;    
}

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

bool _sir_syslog_updated(sirinit* si, sir_update_config_data* data) {
    _SIR_UNUSED(si);
    _SIR_UNUSED(data);
    return false;
}

void _sir_syslog_reset(sir_syslog_dest* ctx) {
    _SIR_UNUSED(ctx);
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
            _sir_selflog("strftime returned 0; format string: '%s'", format);

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
            _sir_selflog("clock_gettime failed; errno: %d", errno);
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
