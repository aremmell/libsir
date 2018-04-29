/*!
 * \file sirconsole.h
 * \brief Internal definitions for console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_CONSOLE_H_INCLUDED
#define _SIR_CONSOLE_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

const sir_textstyle _sir_getdefstyle(sir_level level);
const sir_textstyle_priv _sir_getsysstyle(sir_textstyle style);


#endif /* !_SIR_CONSOLE_H_INCLUDED */
