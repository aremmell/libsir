/**
 * @file sirconsole.h
 * @brief Internal stdio management.
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
