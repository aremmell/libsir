/**
 * @file sirhelpers.c
 * @brief Internal macros and inline functions.
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
#include "sirhelpers.h"
#include "sirerrors.h"

/**
 * @addtogroup intern
 * @{
 */

bool _sir_validfid(int id) {
    bool valid = id >= 0;
    if (!valid) {
        _sir_seterror(_SIR_E_NOFILE);
        assert(valid);
    }
    return valid;
}

bool _sir_validlevels(sir_levels levels) {
    bool valid = levels <= SIRL_ALL;
    if (!valid) {
        _sir_seterror(_SIR_E_LEVELS);
        assert(valid);
    }
    return valid;
}

bool _sir_validlevel(sir_level level) {
    bool valid = 0 != level && !(level & (level - 1));
    if (!valid) {
        _sir_seterror(_SIR_E_LEVELS);
        assert(valid);
    }
    return valid;
}

bool _sir_validopts(sir_options opts) {
    bool valid = (opts & SIRL_ALL) == 0 && opts <= 0xfff00;
    if (!valid) {
        _sir_seterror(_SIR_E_OPTIONS);
        assert(valid);
    }
    return valid;
}

bool __sir_validstr(const sirchar_t* str, bool fail) {
    bool valid = str && (*str != (sirchar_t)'\0');
    if (!valid && fail) {
        _sir_seterror(_SIR_E_STRING);
        assert(valid);
    }
    return valid;
}

/** @} */
