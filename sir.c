/*!
 * \file sir.c
 * 
 * Implementation of the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */
#include "sir.h"
#include "sirmutex.h"
#include "sirmacros.h"
#include "sirinternal.h"
#include "sirfilecache.h"

bool sir_init(const sirinit* si) {

    if (!si || !_sir_options_sanity(si))
        return false;

    sir_cleanup();
    memcpy(&sir_s, si, sizeof(sirinit));

#ifndef SIR_NO_SYSLOG
    // TODO: if not using process name, use pid for syslog identity?
    if (0 != sir_s.sysLogLevels)
        openlog(validstr(sir_s.processName) ? sir_s.processName : "",
            (sir_s.sysLogIncludePID ? LOG_PID : 0) | LOG_ODELAY, LOG_USER);
#endif

#ifdef _WIN32
#ifdef DEBUG
    _set_invalid_parameter_handler(_sir_invalidparam);
#endif
#endif

    return true;
}

bool sirdebug(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirinfo(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirnotice(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirwarn(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_WARN, format, args);
    _SIR_L_END(args);
    return r;
}

bool sirerror(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

bool sircrit(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

bool siralert(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

bool siremerg(const sirchar_t* format, ...) {
    _SIR_L_START(format);
    r = _sir_lv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

void sir_cleanup(void) {
    _sir_files_destroy(&sir_fc);
    memset(&sir_s, 0, sizeof(sirinit));
    memset(&sir_fc, 0, sizeof(sirfiles));
    _sirbuf_reset(&sir_b);
}

int sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    return _sir_files_add(&sir_fc, path, levels, opts);
}

bool sir_remfile(int id) {
    return _sir_files_rem(&sir_fc, id);
}
