/**
 * @file sirconsole.c
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
#include "sirconsole.h"
#include "sirinternal.h"
#include "sirtextstyle.h"

/**
 * @addtogroup intern
 * @{
 */

#ifndef _WIN32

static bool _sir_write_std(const sirchar_t* message, FILE* stream);

bool _sir_stderr_write(const sirchar_t* message) {
    return _sir_write_std(message, stderr);
}

bool _sir_stdout_write(const sirchar_t* message) {
    return _sir_write_std(message, stdout);
}

static bool _sir_write_std(const sirchar_t* message, FILE* stream) {

    if (!_sir_validstr(message) || !_sir_validptr(stream))
        return false;     

    if (EOF == fputs(message, stream)) {
        _sir_handleerr(errno);
        return false;
    }

    return true;
}

#else

static CRITICAL_SECTION stdout_cs;
static sironce_t        stdout_once = SIR_ONCE_INIT;

static CRITICAL_SECTION stderr_cs;
static sironce_t        stderr_once = SIR_ONCE_INIT;

static bool _sir_write_stdwin32(uint16_t style, const sirchar_t* message,
    HANDLE console, CRITICAL_SECTION* cs);
static BOOL CALLBACK _sir_initcs(PINIT_ONCE ponce, PVOID param, PVOID* ctx);

bool _sir_stderr_write(uint16_t style, const sirchar_t* message) {
    BOOL initcs = InitOnceExecuteOnce(&stderr_once, _sir_initcs, &stderr_cs, NULL);
    assert(FALSE != initcs);
    return _sir_write_stdwin32(style, message, GetStdHandle(STD_ERROR_HANDLE), &stderr_cs);
}

bool _sir_stdout_write(uint16_t style, const sirchar_t* message) {
    BOOL initcs = InitOnceExecuteOnce(&stdout_once, _sir_initcs, &stdout_cs, NULL);
    assert(FALSE != initcs);
    return _sir_write_stdwin32(style, message, GetStdHandle(STD_OUTPUT_HANDLE), &stdout_cs);
}

static bool _sir_write_stdwin32(uint16_t style, const sirchar_t* message,
    HANDLE console, CRITICAL_SECTION* cs) {

    if (!_sir_validstr(message))
        return false;

    assert(INVALID_HANDLE_VALUE != console);
    if (INVALID_HANDLE_VALUE == console) {
        _sir_handlewin32err(GetLastError());
        return false;
    }

    if (!_sir_validptr(cs))
        return false;

    CONSOLE_SCREEN_BUFFER_INFO csbfi = {0};

    EnterCriticalSection(cs);
    if (!GetConsoleScreenBufferInfo(console, &csbfi)) {
        _sir_handlewin32err(GetLastError());
        return false;
    }

    if (!SetConsoleTextAttribute(console, style)) {
        _sir_handlewin32err(GetLastError());
        return false;
    }

    size_t chars   = strnlen(message, SIR_MAXOUTPUT) - 1;
    DWORD  written = 0;

    do {
        DWORD pass = 0;

        if (!WriteConsole(console, message + written, chars - written, &pass, NULL)) {
            _sir_handlewin32err(GetLastError());
            break;
        }

        written += pass;
    } while (written < chars);

    /* Have to set the attributes back before writing the newline
     * or you'll get background color artifacting before the next output.
     */
    SetConsoleTextAttribute(console, csbfi.wAttributes);
    DWORD newline = 0;
    WriteConsole(console, "\n", 1, &newline, NULL);
    LeaveCriticalSection(cs);

    return written == chars;
}

static BOOL CALLBACK _sir_initcs(PINIT_ONCE ponce, PVOID param, PVOID* ctx) {
    InitializeCriticalSection((LPCRITICAL_SECTION)param);
    return TRUE;
}

#endif /* !_WIN32 */

/** @} */
