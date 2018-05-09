/**
 * @file sir.c
 * @brief Public interface.
 * 
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 * 
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @version 2.0.0
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
#include "sir.h"
#include "sirinternal.h"
#include "sirfilecache.h"
#include "sirtextstyle.h"

/**
 * @addtogroup public 
 * @{
 */

bool sir_init(sirinit* si) {
    return _sir_init(si);
}

bool sir_cleanup() {
    return _sir_cleanup();
}

uint16_t sir_geterror(sirchar_t message[SIR_MAXERROR]) {
    return _sir_geterrcode(_sir_geterror(message));
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

sirfileid_t sir_addfile(const sirchar_t* path, sir_levels levels, sir_options opts) {
    return _sir_addfile(path, levels, opts);
}

bool sir_remfile(sirfileid_t id) {
    return _sir_remfile(id);
}

bool sir_settextstyle(sir_level level, sir_textstyle style) {
    return _sir_setdefstyle(level, style);
}

/** @} */
