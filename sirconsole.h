/**
 * @file sirconsole.h
 * @brief stdio output.
 */
#ifndef _SIR_CONSOLE_H_INCLUDED
#define _SIR_CONSOLE_H_INCLUDED

#include "sirtypes.h"

/**
 * @addtogroup intern
 * @{
 */

#ifndef _WIN32
bool _sir_stderr_write(const sirchar_t* message);
bool _sir_stdout_write(const sirchar_t* message);
#else
bool _sir_stderr_write(uint16_t style, const sirchar_t* message);
bool _sir_stdout_write(uint16_t style, const sirchar_t* message);
#endif

/** @} */

#endif /* !_SIR_CONSOLE_H_INCLUDED */
