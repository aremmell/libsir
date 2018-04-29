/*!
 * \file sirconsole.h
 * \brief Internal implementation of console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#include "sirconsole.h"
#include "sirinternal.h"
#include "sirdefaults.h"

/* \cond PRIVATE */

const sir_textstyle _sir_getdefstyle(sir_level level) {

    assert(validlevels(level));

    for (size_t n = 0; n < _COUNTOF(sir_default_styles); n++) {
        if (sir_default_styles[n].level == level)
            return sir_default_styles[n].style;
    }

    return SIRS_NONE;
}

const sir_textstyle_priv _sir_combinestyles(sir_textstyle style) {
#ifndef _WIN32
    
#else
    return 0;
#endif
}

/* \endcond PRIVATE */
