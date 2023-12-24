/*
 * console.h
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
 *
 * -----------------------------------------------------------------------------
 */

#ifndef _SIR_CONSOLE_H_INCLUDED
# define _SIR_CONSOLE_H_INCLUDED

# include "sir/types.h"
# include "sir/helpers.h"

# if !defined(__WIN__)
bool _sir_write_stdio(FILE* stream, const char* message);

static inline
bool _sir_write_stdout(const char* message, size_t len) {
    SIR_UNUSED(len);
    return _sir_write_stdio(stdout, message);
}

static inline
bool _sir_write_stderr(const char* message, size_t len) {
    SIR_UNUSED(len);
    return _sir_write_stdio(stderr, message);
}
# else /* __WIN__ */
extern HANDLE __sir_stdout;
extern HANDLE __sir_stderr;

void _sir_initialize_stdio(void);
bool _sir_write_stdio(HANDLE console, const char* message, size_t len);

static inline
bool _sir_write_stdout(const char* message, size_t len) {
    return _sir_write_stdio(__sir_stdout, message, len);
}

static inline
bool _sir_write_stderr(const char* message, size_t len) {
    return _sir_write_stdio(__sir_stderr, message, len);
}
# endif /* !__WIN__ */

#endif /* !_SIR_CONSOLE_H_INCLUDED */
