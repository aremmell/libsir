/*
 * sirerrors.c
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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

#include "sir/errors.h"
#include "sir/helpers.h"

#if defined(__MACOS__) || defined(__BSD__) || \
    (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200112L && !defined(_GNU_SOURCE)))
# define __HAVE_XSI_STRERROR_R__
# if defined(__GLIBC__) && GLIBC_VERSION < 21300
#  define __HAVE_XSI_STRERROR_R_ERRNO__
# endif
#elif defined(_GNU_SOURCE) && defined(__GLIBC__)
# define __HAVE_GNU_STRERROR_R__
#elif defined(__HAVE_STDC_SECURE_OR_EXT1__)
# define __HAVE_STRERROR_S__
#endif

/** Per-thread error data */
static _sir_thread_local sir_thread_err _sir_te = {
    _SIR_E_NOERROR, 0, {0}, {SIR_UNKNOWN, SIR_UNKNOWN, 0}
};

#define _SIR_E_PLATFORM_ERRORMSG "Platform error"
#define _SIR_E_PLATFORM_ERRORFORMAT _SIR_E_PLATFORM_ERRORMSG " code %d: %s"

static const struct {
    uint32_t e;
    const char* msg;
} sir_errors[] = {
    {_SIR_E_NOERROR,   "The operation completed successfully"},
    {_SIR_E_NOTREADY,  "libsir has not been initialized"},
    {_SIR_E_ALREADY,   "libsir is already initialized"},
    {_SIR_E_DUPITEM,   "Item already managed by libsir"},
    {_SIR_E_NOITEM,    "Item not managed by libsir"},
    {_SIR_E_NOROOM,    "Maximum number of items already stored"},
    {_SIR_E_OPTIONS,   "Option flags are invalid"},
    {_SIR_E_LEVELS,    "Level flags are invalid"},
    {_SIR_E_TEXTSTYLE, "Text style is invalid"},
    {_SIR_E_STRING,    "Invalid string argument"},
    {_SIR_E_NULLPTR,   "NULL pointer argument"},
    {_SIR_E_INVALID,   "Invalid argument"},
    {_SIR_E_NODEST,    "No destinations registered for level"},
    {_SIR_E_UNAVAIL,   "Feature is disabled or unavailable"},
    {_SIR_E_INTERNAL,  "An internal error has occurred"},
    {_SIR_E_COLORMODE, "Color mode is invalid"},
    {_SIR_E_TEXTATTR,  "Text attributes are invalid"},
    {_SIR_E_TEXTCOLOR, "Text color is invalid for mode"},
    {_SIR_E_PLUGINBAD, "Plugin module is malformed"},
    {_SIR_E_PLUGINDAT, "Data produced by plugin is invalid"},
    {_SIR_E_PLUGINVER, "Plugin interface version unsupported"},
    {_SIR_E_PLUGINERR, "Plugin reported failure"},
    {_SIR_E_PLATFORM,  _SIR_E_PLATFORM_ERRORFORMAT},
    {_SIR_E_UNKNOWN,   "Unknown error"},
};

bool __sir_seterror(uint32_t err, const char* func, const char* file, uint32_t line) {
    if (_sir_validerror(err)) {
        _sir_te.lasterror = err;
        _sir_te.loc.func  = func;
        _sir_te.loc.file  = file;
        _sir_te.loc.line  = line;

        if (_SIR_E_PLATFORM != err) {
            _sir_te.os_code = 0;
            _sir_resetstr(_sir_te.os_msg);
        }
    }
#if defined(DEBUG) && defined(SIR_SELFLOG)
    if (_SIR_E_NOERROR != err) {
        char errmsg[SIR_MAXERROR] = {0};
        _sir_geterror(errmsg);
        __sir_selflog(func, file, line, "%s", errmsg);
    }
#endif
    return false;
}

void __sir_setoserror(int code, const char* msg, const char* func, const char* file, uint32_t line) {
    _sir_te.os_code = code;
    _sir_resetstr(_sir_te.os_msg);

    if (_sir_validstrnofail(msg))
        (void)_sir_strncpy(_sir_te.os_msg, SIR_MAXERROR, msg, SIR_MAXERROR);

    (void)__sir_seterror(_SIR_E_PLATFORM, func, file, line);
}

bool __sir_handleerr(int code, const char* func, const char* file, uint32_t line) {
    if (SIR_E_NOERROR != code) {
        char message[SIR_MAXERROR] = {0};
        int finderr                = 0;
        errno                      = SIR_E_NOERROR;

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
        const char* tmp = strerror_r(code, message, SIR_MAXERROR);
        if (tmp != message)
            (void)_sir_strncpy(message, SIR_MAXERROR, tmp, SIR_MAXERROR);
#elif defined(__HAVE_STRERROR_S__)
        _sir_selflog("using strerror_s");
        finderr = (int)strerror_s(message, SIR_MAXERROR, code);
#else
        _sir_selflog("using strerror");
        const char* tmp = strerror(code);
        (void)_sir_strncpy(message, SIR_MAXERROR, tmp, strnlen(tmp, SIR_MAXERROR));
#endif
        if (0 == finderr) { //-V547
            __sir_setoserror(code, message, func, file, line);
#if defined(__HAVE_XSI_STRERROR_R__) || defined(__HAVE_STRERROR_S__)
        } else {
            /* Per my reading of the man pages, GNU strerror_r and normal strerror "can't fail";
             * they simply return a string such as "Unknown nnn error" if unable to look up an
             * error code.
             */
# if defined(__HAVE_XSI_STRERROR_R__)
            _sir_selflog("strerror_r failed! error: %d", finderr);
# elif defined(__HAVE_STRERROR_S__)
            _sir_selflog("strerror_s failed! error: %d", finderr);
# endif
#endif
        }
    }
    return false;
}

#if defined(__WIN__)
void _sir_invalidparameter(const wchar_t* expr, const wchar_t* func, const wchar_t* file,
    unsigned int line, uintptr_t reserved) {
    _sir_selflog("invalid parameter handler: expression: '%S' in %S (%S:%u)",
        expr, func, file, line);
    SIR_UNUSED(reserved);
}

bool __sir_handlewin32err(DWORD code, const char* func, const char* file, uint32_t line) {
    char* errbuf = NULL;
    DWORD flags  = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                   FORMAT_MESSAGE_IGNORE_INSERTS  | FORMAT_MESSAGE_MAX_WIDTH_MASK;

    DWORD fmtmsg = FormatMessageA(flags, NULL, code, 0, (LPSTR)&errbuf, SIR_MAXERROR, NULL);
    if (0 < fmtmsg && _sir_validstrnofail(errbuf)) {
        if (errbuf[fmtmsg - 1] == '\n' || errbuf[fmtmsg - 1] == ' ')
            errbuf[fmtmsg - 1] = '\0';
        __sir_setoserror((int)code, errbuf, func, file, line);
    } else {
        _sir_selflog("FormatMessage failed! error: %lu", GetLastError());
        SIR_ASSERT(false);
    }

    if (errbuf) {
        HLOCAL local_free = LocalFree((HLOCAL)errbuf);
        SIR_ASSERT_UNUSED(NULL == local_free, local_free);
        errbuf = NULL;
    }
    return false;
}
#endif /* !__WIN__ */

uint32_t _sir_geterror(char message[SIR_MAXERROR]) {
    _sir_resetstr(message);

    static const size_t low  = 0;
    static const size_t high = _sir_countof(sir_errors) - 1;

    uint32_t retval = _SIR_E_UNKNOWN;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH()

    if (sir_errors[_mid].e == _sir_te.lasterror) {
        char* heap_msg = NULL;

        if (_SIR_E_PLATFORM == sir_errors[_mid].e) {
            heap_msg = calloc(SIR_MAXERROR, sizeof(char));
            if (_sir_validptrnofail(heap_msg)) {
                _sir_snprintf_trunc(heap_msg, SIR_MAXERROR, _SIR_E_PLATFORM_ERRORFORMAT, _sir_te.os_code,
                    (_sir_validstrnofail(_sir_te.os_msg) ? _sir_te.os_msg : SIR_UNKNOWN));
            }
        }

        _sir_snprintf_trunc(message, SIR_MAXERROR, SIR_ERRORFORMAT, _sir_te.loc.func, //-V576
            _sir_te.loc.file, _sir_te.loc.line, (_sir_validstrnofail(heap_msg)
                ? heap_msg : (_sir_validstrnofail(sir_errors[_mid].msg)
                ? sir_errors[_mid].msg : SIR_UNKNOWN)));

        _sir_safefree(&heap_msg);

        retval = sir_errors[_mid].e;
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((sir_errors[_mid].e < _sir_te.lasterror ? 1 : -1));
    _SIR_END_BIN_SEARCH();

    return retval;
}

void _sir_geterrorinfo(sir_errorinfo* err) {
    if (!_sir_validptr(err))
        return;

    (void)memset(err, 0, sizeof(sir_errorinfo));

    err->func = _sir_te.loc.func;
    err->file = _sir_te.loc.file;
    err->line = _sir_te.loc.line;

    err->os_code = _sir_te.os_code;
    (void)_sir_strncpy(err->os_msg, SIR_MAXERROR, (_sir_validstrnofail(_sir_te.os_msg)
        ? _sir_te.os_msg : SIR_UNKNOWN), SIR_MAXERROR);

    err->code = _sir_geterrcode(SIR_E_UNKNOWN);

    static const size_t low  = 0;
    static const size_t high = _sir_countof(sir_errors) - 1;

    _SIR_DECLARE_BIN_SEARCH(low, high);
    _SIR_BEGIN_BIN_SEARCH()

    if (sir_errors[_mid].e == _sir_te.lasterror) {
        err->code = _sir_geterrcode(sir_errors[_mid].e);
        if (_SIR_E_PLATFORM == sir_errors[_mid].e)
            (void)_sir_strncpy(err->msg, SIR_MAXERROR, _SIR_E_PLATFORM_ERRORMSG, SIR_MAXERROR);
        else
            (void)_sir_strncpy(err->msg, SIR_MAXERROR, sir_errors[_mid].msg, SIR_MAXERROR);
        break;
    }

    _SIR_ITERATE_BIN_SEARCH((sir_errors[_mid].e < _sir_te.lasterror ? 1 : -1));
    _SIR_END_BIN_SEARCH();
}

void _sir_reset_tls_error(void) {
    _sir_te.lasterror = _SIR_E_NOERROR;
    _sir_te.os_code   = 0;
    _sir_resetstr(_sir_te.os_msg);
    _sir_te.loc.func = SIR_UNKNOWN;
    _sir_te.loc.file = SIR_UNKNOWN;
    _sir_te.loc.line = 0U;
}

#if defined(SIR_SELFLOG)
PRINTF_FORMAT_ATTR(4, 5)
void __sir_selflog(const char* func, const char* file, uint32_t line,
    PRINTF_FORMAT const char* format, ...) {
    bool success = true;
    char prefix[256];

    int write1 = snprintf(prefix, 256, "%s (%s:%"PRIu32"): ", func, file, line);
    _sir_eqland(success, write1 > 0);

    if (write1 > 0) {
        va_list args;
        va_list args2;
        va_start(args, format);
        va_copy(args2, args);

        int write2 = vsnprintf(NULL, 0, format, args);
        va_end(args);
        _sir_eqland(success, write2 > 0);

        if (write2 > 0) {
            char* buf = (char*)malloc(write2 + 1);
            _sir_eqland(success, NULL != buf);

            if (buf) {
                write2 = vsnprintf(buf, write2 + 1, format, args2);
                va_end(args2);
                _sir_eqland(success, write2 > 0);

                bool error = false;
                bool warn  = false;
                if (write2 > 0) {
# if !defined(__WIN__)
                    if (NULL != strcasestr(buf, "error") ||
                        NULL != strcasestr(buf, "assert")) {
# else /* __WIN__ */
                    if (NULL != StrStrIA(buf, "error") ||
                        NULL != StrStrIA(buf, "assert")) {
# endif
                        error = true;
# if !defined(__WIN__)
                    } else if (NULL != strcasestr(buf, "warn")) {
# else /* __WIN__ */
                    } else if (NULL != StrStrIA(buf, "warn")) {
# endif
                        warn = true;
                    }
                    write2 = fprintf(stderr, (error ? SIR_BRED("%s%s") SIR_EOL :
                        (warn ? SIR_YELLOW("%s%s") SIR_EOL : "%s%s" SIR_EOL)), prefix, buf);
                    _sir_eqland(success, write2 > 0);
                }

                _sir_safefree(&buf);
            }
        }
    }

    SIR_ASSERT_UNUSED(success, success);
}
#endif
