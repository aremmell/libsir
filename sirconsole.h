/*!
 * \file sirconsole.h
 * \brief Internal definitions for console color management in the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_CONSOLE_H_INCLUDED
#define _SIR_CONSOLE_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

bool             _sir_stderr_write(const sirchar_t* message);
bool             _sir_stdout_write(const sirchar_t* message);

const sir_textstyle _sir_getdefstyle(sir_level level);
const sir_textstyle_priv _sir_getsysstyle(uint32_t styles);

#endif /* !_SIR_CONSOLE_H_INCLUDED */
