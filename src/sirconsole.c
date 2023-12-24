/*
 * sirconsole.c
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

#include "sir/console.h"
#include "sir/internal.h"

#if !defined(__WIN__)
bool _sir_write_stdio(FILE* stream, const char* message) {
    return (EOF != fputs(message, stream)) ? true : _sir_handleerr(errno);
}
#else /* __WIN__ */
HANDLE __sir_stdout  = INVALID_HANDLE_VALUE;
HANDLE __sir_stderr  = INVALID_HANDLE_VALUE;
static sir_once config_once = SIR_ONCE_INIT;

static
bool _sir_config_console(HANDLE console) {
    if (INVALID_HANDLE_VALUE == console || NULL == console)
        return _sir_handlewin32err(GetLastError());

    DWORD mode = 0UL;
    if (!GetConsoleMode(console, &mode))
        return _sir_handlewin32err(GetLastError());

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_PROCESSED_OUTPUT;
    if (!SetConsoleMode(console, mode))
        return _sir_handlewin32err(GetLastError());

    return true;
}

static
BOOL CALLBACK __sir_config_consoles_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    SIR_UNUSED(ponce);
    SIR_UNUSED(param);
    SIR_UNUSED(ctx);

    __sir_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
    __sir_stderr = GetStdHandle(STD_ERROR_HANDLE);

    return (_sir_config_console(__sir_stdout) && _sir_config_console(__sir_stderr))
        ? TRUE : FALSE;
}

void _sir_initialize_stdio(void) {
    if (!_sir_once(&config_once, __sir_config_consoles_once))
        _sir_selflog("warning: unable to configure stdio consoles!");
}

bool _sir_write_stdio(HANDLE console, const char* message, size_t len) {
    DWORD chars   = (DWORD)len;
    DWORD written = 0UL;

    do {
        DWORD pass = 0UL;

        if (!WriteConsole(console, message + written, chars - written, &pass, NULL))
            return _sir_handlewin32err(GetLastError());

        written += pass;
    } while (written < chars);

    return written == chars;
}
#endif /* !__WIN__ */
