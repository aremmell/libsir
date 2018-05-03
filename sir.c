/*!
 * \file sir.c
 * \brief Implementation of the public interface to the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sir.h"
#include "sirdefaults.h"
#include "sirfilecache.h"
#include "sirinternal.h"
#include "sirmacros.h"
#include "sirmutex.h"
#include "sirtextstyle.h"

/*! \fn bool sir_init(const sirinit* si) */
bool sir_init(const sirinit* si) {
    return _sir_init(si);
}

/*! \fn bool sir_cleanup() */
bool sir_cleanup() {
    return _sir_cleanup();
}

/*! \fn bool sir_debug(const sirchar_t* format, ...) */
bool sir_debug(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_info(const sirchar_t* format, ...) */
bool sir_info(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_notice(const sirchar_t* format, ...) */
bool sir_notice(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_warn(const sirchar_t* format, ...) */
bool sir_warn(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_WARN, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_error(const sirchar_t* format, ...) */
bool sir_error(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_crit(const sirchar_t* format, ...) */
bool sir_crit(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_alert(const sirchar_t* format, ...) */
bool sir_alert(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn bool sir_emerg(const sirchar_t* format, ...) */
bool sir_emerg(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_logv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

/*! \fn int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) */
int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    return _sir_addfile(path, levels, opts);
}

/*! \fn sir_remfile(int id) */
bool sir_remfile(int id) {
    return _sir_remfile(id);
}

/*! \fn bool sir_settextstyle(sir_level level, sir_textstyle style) */
bool sir_settextstyle(sir_level level, sir_textstyle style) {
    return _sir_setdefstyle(level, style);
}
