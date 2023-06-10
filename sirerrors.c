/*
 * sirerrors.c
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
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
#include "sirerrors.h"

#if defined(__WIN__)
# pragma comment(lib, "Shlwapi.lib")
#endif

#if defined(__MACOS__) || defined(__BSD__) || (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L && !defined(_GNU_SOURCE)))
#   define __HAVE_XSI_STRERROR_R__
#   if defined(__GLIBC__)
#       if (__GLIBC__ >= 2 && __GLIBC__MINOR__ < 13)
#           define __HAVE_XSI_STRERROR_R_ERRNO__
#       endif
#   endif
#elif defined(_GNU_SOURCE)
#   define __HAVE_GNU_STRERROR_R__
#elif defined(__HAVE_STDC_SECURE_OR_EXT1__)
#   define __HAVE_STRERROR_S__
#endif

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
}

void __sir_setoserror(int code, const sirchar_t* message, const sirchar_t* func,
                      const sirchar_t* file, uint32_t line) {
    sir_te.os_error = code;
    _sir_resetstr(sir_te.os_errmsg);

    if (_sir_validstrnofail(message))
        _sir_strncpy(sir_te.os_errmsg, SIR_MAXERROR, message, SIR_MAXERROR);

    __sir_seterror(_SIR_E_PLATFORM, func, file, line);
}

void __sir_handleerr(int code, const sirchar_t* func, const sirchar_t* file, uint32_t line) {
    if (SIR_E_NOERROR != code) {
        sirchar_t message[SIR_MAXERROR] = {0};
        int finderr = 0;
        errno = SIR_E_NOERROR;

#if defined(__HAVE_XSI_STRERROR_R__)
        _sir_selflog("using XSI strerror_r");
        finderr = strerror_r(code, message, SIR_MAXERROR);
# if defined(__HAVE_XSI_STRERROR_R_ERRNO__)
        _sir_selflog("using XSI strerror_r for glibc < 2.13");
        if (finderr == -1)
            finderr = errno;
# endif
#elif defined(__HAVE_GNU_STRERROR_R__)
        _sir_selflog("using GNU strerror_r");
        char* tmp = strerror_r(code, message, SIR_MAXERROR);
        if (tmp != message)
            _sir_strncpy(message, SIR_MAXERROR, tmp, SIR_MAXERROR);
#elif defined(__HAVE_STRERROR_S__)
        _sir_selflog("using strerror_s");
        finderr = (int)strerror_s(message, SIR_MAXERROR, code);
#else
        _sir_selflog("using strerror");
        char* tmp = strerror(code);
        _sir_strncpy(message, SIR_MAXERROR, tmp, strnlen(tmp, SIR_MAXERROR));
#endif
        if (0 == finderr && _sir_validstrnofail(message)) {
            __sir_setoserror(code, message, func, file, line);
        } else {
            /* Per my reading of the man pages, GNU strerror_r and normal strerror "can't fail";
             * they simply return a string such as "Unknown nnn error" if unable to look up an
             * error code.
             */
#if defined(__HAVE_XSI_STRERROR_R__)
            _sir_selflog("strerror_r failed! error: %d", finderr);
#elif defined(__HAVE_STRERROR_S__)
            _sir_selflog("strerror_s failed! error: %d", finderr);
#endif
        }
    }
}

#if defined(__WIN__)
void __sir_handlewin32err(DWORD code, const sirchar_t* func, const sirchar_t* file, uint32_t line) {
    sirchar_t* errbuf = NULL;
    DWORD flags       = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS  | FORMAT_MESSAGE_MAX_WIDTH_MASK;

    DWORD fmtmsg = FormatMessageA(flags, NULL, code, 0, (LPSTR)&errbuf, SIR_MAXERROR, NULL);
    if (0 < fmtmsg && _sir_validstrnofail(errbuf)) {
        if (errbuf[fmtmsg - 1] == '\n' || errbuf[fmtmsg - 1] == ' ')
            errbuf[fmtmsg - 1] = '\0';
        __sir_setoserror((int)code, errbuf, func, file, line);
    } else {
        _sir_selflog("FormatMessage failed! error: %d", GetLastError());
        assert(false);
    }

    if (errbuf) {
        HLOCAL local_free = LocalFree((HLOCAL)errbuf);
        _SIR_UNUSED(local_free);
        assert(NULL == local_free);
        errbuf = NULL;
    }
}
#endif

sirerror_t _sir_geterror(sirchar_t message[SIR_MAXERROR]) {
    _sir_resetstr(message);

    size_t low  = 0;
    size_t high = _sir_countof(sir_errors) - 1;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH();

    if (sir_errors[_mid].e == sir_te.lasterror) {
        sirchar_t* final = NULL;
        bool alloc = false;

        if (_SIR_E_PLATFORM == sir_errors[_mid].e) {
            final = (sirchar_t*)calloc(SIR_MAXERROR, sizeof(sirchar_t));

            if (_sir_validptr(final)) {
                alloc = true;
                snprintf(final, SIR_MAXERROR, sir_errors[_mid].msg, sir_te.os_error,
                    (_sir_validstrnofail(sir_te.os_errmsg) ? sir_te.os_errmsg : SIR_UNKNOWN));
            }
        } else {
            final = (sirchar_t*)sir_errors[_mid].msg;
        }

        int fmtmsg = snprintf(message, SIR_MAXERROR, SIR_ERRORFORMAT, sir_te.loc.func,
            sir_te.loc.file, sir_te.loc.line, final);

        _SIR_UNUSED(fmtmsg);
        assert(fmtmsg >= 0);

        if (alloc)
            _sir_safefree(final);

        return sir_errors[_mid].e;
    }

    int comparison = sir_errors[_mid].e < sir_te.lasterror ? 1 : -1;

    _SIR_ITERATE_BIN_SEARCH(comparison);
    _SIR_END_BIN_SEARCH();

    assert(false && sir_te.lasterror);
    return _SIR_E_UNKNOWN;
}

#if defined(SIR_SELFLOG)
void __sir_selflog(const char* func, const char* file, uint32_t line, const char* format, ...) {
    bool success     = true;
    char prefix[256] = {0};

    int wrote = snprintf(prefix, 256, "%s (%s:%" PRIu32 "): ", func, file, line);
    success &= wrote > 0;
        
    if (wrote > 0) {
        va_list args;
        va_list args2;
        va_start(args, format);
        va_copy(args2, args);

        wrote = vsnprintf(NULL, 0, format, args);
        va_end(args);
        success &= wrote > 0;

        if (wrote > 0) {
            char *buf = (char*)malloc(wrote + 1);
            success &= NULL != buf;

            if (buf) {
                wrote = vsnprintf(buf, wrote + 1, format, args2);
                va_end(args2);
                success &= wrote > 0;

                int put = fputs(prefix, stderr);
                success &= put != EOF;

                put = fputs(buf, stderr);
                success &= put != EOF;

                put = fputs("\n", stderr);
                success &= put != EOF;

                _sir_safefree(buf);
            }
        }
    }

    assert(success);
}
#endif
