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
    return _sir_init(si);
}

bool sir_cleanup() {
    return _sir_cleanup();
}

bool sir_debug(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_info(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_notice(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_warn(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_WARN, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_error(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_crit(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_alert(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

bool sir_emerg(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);

    if (sfc) {
        bool r = _sir_fcache_add(sfc, path, levels, opts);
        return r && _sir_unlocksection(_SIRM_FILECACHE);
    }

    return false;
}

bool sir_remfile(int id) {
    sirfcache* sfc = _sir_locksection(_SIRM_FILECACHE);

    if (sfc) {
        bool r = _sir_fcache_rem(sfc, id);
        return r && _sir_unlocksection(_SIRM_FILECACHE);
    }

    return false;
}

bool sir_settextstyle(sir_level level, sir_textstyle style) {
    return _sir_setdefstyle(level, style);
}
