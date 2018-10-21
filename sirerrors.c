/**
 * @file sirerrors.c
 * @brief Error management.
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
#include "sirerrors.h"

/**
 * @addtogroup errors
 * @{
 */

/** Per-thread error data */
static thread_local sir_thread_err sir_te = {
    _SIR_E_NOERROR, 0, {0}, {SIR_UNKNOWN, SIR_UNKNOWN, 0}
};

void __sir_seterror(sirerror_t err, const sirchar_t* func, const sirchar_t* file, uint32_t line) {
    if (_sir_validerror(err)) {
        sir_te.lasterror = err;
        sir_te.loc.func = func;
        sir_te.loc.file = file;
        sir_te.loc.line = line;
    }
    assert(_SIR_E_NOERROR == err);
}

void __sir_setoserror(int code, const sirchar_t* message, const sirchar_t* func,
                      const sirchar_t* file, uint32_t line) {
    sir_te.os_error = code;
    _sir_resetstr(sir_te.os_errmsg);

    if (_sir_validstrnofail(message))
        strncpy(sir_te.os_errmsg, message, SIR_MAXERROR);

    __sir_seterror(_SIR_E_PLATFORM, func, file, line);
}

void __sir_handleerr(int code, const sirchar_t* func, const sirchar_t* file, uint32_t line) {
    if (SIR_E_NOERROR != code) {
        sirchar_t message[SIR_MAXERROR] = {0};

#ifndef _WIN32
    errno = SIR_E_NOERROR;
#   if _POSIX_C_SOURCE >= 200112L && !defined(_GNU_SOURCE)
        int finderr = strerror_r(code, message, SIR_MAXERROR);
#   else
        int finderr = 0;
        char* tmp = strerror_r(code, message, SIR_MAXERROR);
        if (tmp != message)
            strncpy(message, tmp, strnlen(tmp, SIR_MAXERROR - 1));
#   endif
#else
        errno_t finderr = strerror_s(message, SIR_MAXERROR, code);
#endif
        if (0 == finderr && _sir_validstrnofail(message)) {
            __sir_setoserror(code, message, func, file, line);
        } else {
#ifndef _WIN32
            _sir_selflog("%s: strerror_r failed! error: %d\n", __func__, errno);
#else
            _sir_selflog("%s: strerror_s failed! error: %d\n", __func__, finderr);
#endif
        }
    }
    assert(SIR_E_NOERROR == code);
}

#ifdef _WIN32
void __sir_handlewin32err(DWORD code, const sirchar_t* func, const sirchar_t* file, uint32_t line) {
    if (ERROR_SUCCESS != code) {
        sirchar_t* errbuf = NULL;
        DWORD      flags  = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK;
        DWORD fmtmsg = FormatMessageA(flags, NULL, code, 0, (LPSTR)&errbuf, SIR_MAXERROR, NULL);

        if (0 == fmtmsg && _sir_validstrnofail(errbuf)) {
            __sir_setoserror((int)code, errbuf, func, file, line);
        } else {
            _sir_selflog("%s: FormatMessage failed! error: %d\n", __func__, GetLastError());
            assert(false);
        }

        if (errbuf) {
            BOOL heapfree = HeapFree(GetProcessHeap(), 0, errbuf);
            assert(0 != heapfree);
            errbuf = NULL;
        }
    }
    assert(ERROR_SUCCESS == code);
}
#endif

sirerror_t _sir_geterror(sirchar_t message[SIR_MAXERROR]) {
    _sir_resetstr(message);
    for (size_t n = 0; n < _sir_countof(sir_errors); n++) {
        if (sir_errors[n].e == sir_te.lasterror) {
            sirchar_t* final = NULL;
            bool alloc = false;

            if (_SIR_E_PLATFORM == sir_errors[n].e) {
                final = (sirchar_t*)calloc(SIR_MAXERROR, sizeof(sirchar_t));

                if (_sir_validptr(final)) {
                    alloc = true;
                    snprintf(final, SIR_MAXERROR, sir_errors[n].msg, sir_te.os_error,
                        _sir_validstrnofail(sir_te.os_errmsg) ? sir_te.os_errmsg : SIR_UNKNOWN);
                }
            } else {
                final = (sirchar_t*)sir_errors[n].msg;
            }

            int fmtmsg = snprintf(message, SIR_MAXERROR, SIR_ERRORFORMAT, sir_te.loc.func, sir_te.loc.file, sir_te.loc.line, final);
            assert(fmtmsg >= 0);

            if (alloc) _sir_safefree(final);
            return sir_errors[n].e;
        }
    }

    assert(false && sir_te.lasterror);
    return _SIR_E_UNKNOWN;
}

#ifdef SIR_SELFLOG
void _sir_selflog(const sirchar_t* format, ...) {
    sirchar_t output[SIR_MAXMESSAGE] = {0};
    va_list   args;

    va_start(args, format);
    int print = vsnprintf(output, SIR_MAXMESSAGE, format, args);
    va_end(args);

    assert(print > 0);

    if (print > 0) {
        int put = fputs(output, stderr);
        assert(put != EOF);
    }
}
#endif

/** @} */
