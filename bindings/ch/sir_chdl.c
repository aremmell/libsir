/*
 * sir_chdl.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.4
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
//-V::1049
#include <ch.h>
#include <stdbool.h>
#include "chsir.h"

EXPORTCH bool
sir_makeinit_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirinit *si;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    si     = Ch_VaArg(interp, ap, sirinit *);
    retval = sir_makeinit(si);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_init_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    sirinit *si;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    si     = Ch_VaArg(interp, ap, sirinit *);
    retval = sir_init(si);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_cleanup_chdl(void) {

    return sir_cleanup();
}

EXPORTCH bool
sir_isprerelease_chdl(void) {

    return sir_isprerelease();
}

EXPORTCH bool
sir_isinitialized_chdl(void) {

    return sir_isinitialized();
}

EXPORTCH uint16_t
sir_geterror_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    char *message;
    uint16_t retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, char *);
    retval  = sir_geterror(message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH const char *
sir_getversionstring_chdl(void) {

    return sir_getversionstring();
}

EXPORTCH uint32_t
sir_getversionhex_chdl(void) {

    return sir_getversionhex();
}

EXPORTCH bool
sir_debug_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_debug("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_info_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_info("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_warn_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_warn("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_alert_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_alert("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_notice_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_notice("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_error_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_error("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_crit_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_crit("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

EXPORTCH bool
sir_emerg_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *message;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    message = Ch_VaArg(interp, ap, const char *);
    retval  = sir_emerg("%s", message);

    Ch_VaEnd(interp, ap);

    return retval;
}

#if !defined(_SIR_INTERNAL_H_INCLUDED)
bool _sir_setthreadname(const char *name);
#endif /* if !defined(_SIR_INTERNAL_H_INCLUDED) */

EXPORTCH bool
_sir_setthreadname_chdl(void *varg) {
    ChInterp_t interp;
    ChVaList_t ap;
    const char *name;
    bool retval;

    Ch_VaStart(interp, ap, varg);

    name   = Ch_VaArg(interp, ap, const char *);
    retval = _sir_setthreadname(name);

    Ch_VaEnd(interp, ap);

    return retval;
}
