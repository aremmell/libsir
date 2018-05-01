/*!
 * \file sir.c
 * \brief Implementation of the public interface to the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sir.h"
#include "sirmacros.h"
#include "sirmutex.h"
#include "sirinternal.h"
#include "sirfilecache.h"
#include "sirtextstyle.h"
#include "sirdefaults.h"

bool sir_init(const sirinit* si) {

    if (_sir_sanity()) {
        _sir_selflog("%s: sir appears to already be initialized!\n", __func__);
        return false;
    }

    if (!si || !_sir_options_sanity(si))
        return false;

    sir_cleanup();
    memcpy(&sir_s, si, sizeof(sirinit));

#ifndef SIR_NO_SYSLOG
    // TODO: if not using process name, use pid for syslog identity?
    if (0 != sir_s.d_syslog.levels)
        openlog(validstr(sir_s.processName) ? sir_s.processName : "",
            (sir_s.d_syslog.includePID ? LOG_PID : 0) | LOG_ODELAY, LOG_USER);
#endif

    sir_magic = _SIR_MAGIC;
    _sir_selflog("SIR is initialized.\n");

    return true;
}

void sir_cleanup() {

    _sir_fcache_destroy(&sir_fc);
    memset(&sir_fc, 0, sizeof(sirfcache));
    _sirbuf_reset(&sir_b);
    memset(&sir_s, 0, sizeof(sirinit));
    
    _sir_selflog("SIR is cleaned up.\n");
}

bool sirdebug(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirinfo(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirnotice(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirwarn(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_WARN, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirerror(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

bool sircrit(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

bool siralert(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

bool siremerg(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    return _sir_fcache_add(&sir_fc, path, levels, opts);
}

bool sir_remfile(int id) {
    return _sir_fcache_rem(&sir_fc, id);
}

bool sir_settextstyle(sir_level level, sir_textstyle style) {
    return _sir_setdefstyle(level, style);
}
