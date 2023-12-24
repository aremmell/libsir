/*
 * errors.h
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

#ifndef _SIR_ERRORS_H_INCLUDED
# define _SIR_ERRORS_H_INCLUDED

# include "sir/types.h"

/**
 * @addtogroup publictypes Types
 * @{
 */

/** Error codes. */
enum sir_errorcode {
    SIR_E_NOERROR   = 1,    /**< The operation completed successfully */
    SIR_E_NOTREADY  = 2,    /**< libsir has not been initialized */
    SIR_E_ALREADY   = 3,    /**< libsir is already initialized */
    SIR_E_DUPITEM   = 4,    /**< Item already managed by libsir */
    SIR_E_NOITEM    = 5,    /**< Item not managed by libsir */
    SIR_E_NOROOM    = 6,    /**< Maximum number of items already stored */
    SIR_E_OPTIONS   = 7,    /**< Option flags are invalid */
    SIR_E_LEVELS    = 8,    /**< Level flags are invalid */
    SIR_E_TEXTSTYLE = 9,    /**< Text style is invalid */
    SIR_E_STRING    = 10,   /**< Invalid string argument */
    SIR_E_NULLPTR   = 11,   /**< NULL pointer argument */
    SIR_E_INVALID   = 12,   /**< Invalid argument */
    SIR_E_NODEST    = 13,   /**< No destinations registered for level */
    SIR_E_UNAVAIL   = 14,   /**< Feature is disabled or unavailable */
    SIR_E_INTERNAL  = 15,   /**< An internal error has occurred */
    SIR_E_COLORMODE = 16,   /**< Color mode is invalid */
    SIR_E_TEXTATTR  = 17,   /**< Text attributes are invalid */
    SIR_E_TEXTCOLOR = 18,   /**< Text color is invalid for mode */
    SIR_E_PLUGINBAD = 19,   /**< Plugin module is malformed */
    SIR_E_PLUGINDAT = 20,   /**< Data produced by plugin is invalid */
    SIR_E_PLUGINVER = 21,   /**< Plugin interface version unsupported */
    SIR_E_PLUGINERR = 22,   /**< Plugin reported failure */
    SIR_E_PLATFORM  = 23,   /**< Platform error code %%d: %%s */
    SIR_E_UNKNOWN   = 4095, /**< Unknown error */
};

/** @} */

# if defined(__cplusplus)
extern "C" {
# endif

/** Creates an error code that (hopefully) doesn't conflict with any of those
 * defined by the platform. */
# define _sir_mkerror(code) (((uint32_t)((code) & 0x7fffffffU) << 16) | 0x80000000U)

/** Validates an internal error. */
static inline
bool _sir_validerror(uint32_t err) {
    uint32_t masked = err & 0x8fffffffU;
    return masked >= 0x80000000U && masked <= 0x8fff0000U;
}

/** Extracts just the code from an internal error. */
static inline
uint16_t _sir_geterrcode(uint32_t err) {
    return (err >> 16) & 0x7fffU;
}

/** Internal error codes. */
# define _SIR_E_NOERROR   _sir_mkerror(SIR_E_NOERROR)
# define _SIR_E_NOTREADY  _sir_mkerror(SIR_E_NOTREADY)
# define _SIR_E_ALREADY   _sir_mkerror(SIR_E_ALREADY)
# define _SIR_E_DUPITEM   _sir_mkerror(SIR_E_DUPITEM)
# define _SIR_E_NOITEM    _sir_mkerror(SIR_E_NOITEM)
# define _SIR_E_NOROOM    _sir_mkerror(SIR_E_NOROOM)
# define _SIR_E_OPTIONS   _sir_mkerror(SIR_E_OPTIONS)
# define _SIR_E_LEVELS    _sir_mkerror(SIR_E_LEVELS)
# define _SIR_E_TEXTSTYLE _sir_mkerror(SIR_E_TEXTSTYLE)
# define _SIR_E_STRING    _sir_mkerror(SIR_E_STRING)
# define _SIR_E_NULLPTR   _sir_mkerror(SIR_E_NULLPTR)
# define _SIR_E_INVALID   _sir_mkerror(SIR_E_INVALID)
# define _SIR_E_NODEST    _sir_mkerror(SIR_E_NODEST)
# define _SIR_E_UNAVAIL   _sir_mkerror(SIR_E_UNAVAIL)
# define _SIR_E_INTERNAL  _sir_mkerror(SIR_E_INTERNAL)
# define _SIR_E_COLORMODE _sir_mkerror(SIR_E_COLORMODE)
# define _SIR_E_TEXTATTR  _sir_mkerror(SIR_E_TEXTATTR)
# define _SIR_E_TEXTCOLOR _sir_mkerror(SIR_E_TEXTCOLOR)
# define _SIR_E_PLUGINBAD _sir_mkerror(SIR_E_PLUGINBAD)
# define _SIR_E_PLUGINDAT _sir_mkerror(SIR_E_PLUGINDAT)
# define _SIR_E_PLUGINVER _sir_mkerror(SIR_E_PLUGINVER)
# define _SIR_E_PLUGINERR _sir_mkerror(SIR_E_PLUGINERR)
# define _SIR_E_PLATFORM  _sir_mkerror(SIR_E_PLATFORM)
# define _SIR_E_UNKNOWN   _sir_mkerror(SIR_E_UNKNOWN)

bool __sir_seterror(uint32_t err, const char* func, const char* file, uint32_t line);
# define _sir_seterror(err) __sir_seterror(err, __func__, __file__, __LINE__)

void __sir_setoserror(int code, const char* msg, const char* func,
    const char* file, uint32_t line);

/** Handle an OS/libc error. */
bool __sir_handleerr(int code, const char* func, const char* file, uint32_t line);
# define _sir_handleerr(code) __sir_handleerr(code, __func__, __file__, __LINE__)

# if defined(__WIN__)
void _sir_invalidparameter(const wchar_t* expr, const wchar_t* func, const wchar_t* file,
    unsigned int line, uintptr_t reserved);

bool __sir_handlewin32err(DWORD code, const char* func, const char* file, uint32_t line);
#  define _sir_handlewin32err(code) __sir_handlewin32err((DWORD)code, __func__, __file__, __LINE__)
# endif

/** Retrieves a formatted message for the last error that occurred on the calling
 * thread and returns the associated internal error code. */
uint32_t _sir_geterror(char message[SIR_MAXERROR]);

/** Returns information about the last error that occurred on the calling thread. */
void _sir_geterrorinfo(sir_errorinfo* err);

/** Resets TLS error information. */
void _sir_reset_tls_error(void);

# if defined(SIR_SELFLOG)
/** Log an internal debug message to stderr. */
PRINTF_FORMAT_ATTR(4, 5)
void __sir_selflog(const char* func, const char* file, uint32_t line, PRINTF_FORMAT const char* format, ...);
#  define _sir_selflog(...) __sir_selflog(__func__, __file__, __LINE__, __VA_ARGS__)
# else
static inline
void __sir_fakefunc(const char* format, ...) { (void)format; }
#  define _sir_selflog(...) __sir_fakefunc(__VA_ARGS__)
# endif

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_ERRORS_H_INCLUDED */
