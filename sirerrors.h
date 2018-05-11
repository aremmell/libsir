/**
 * @file sirerrors.h
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
#ifndef _SIR_ERRORS_H_INCLUDED
#define _SIR_ERRORS_H_INCLUDED

#include "sirhelpers.h"
#include "sirtypes.h"

/** 
 * @defgroup errors Error handling
 * 
 * Functions, types, and codes used for error handling.
 * 
 * @addtogroup errors 
 * @{
 */

/** Error codes. */
enum {
    SIR_E_NOERROR   = 0,    /**< The operation completed successfully */
    SIR_E_NOTREADY  = 1,    /**< libsir has not been initialized */
    SIR_E_ALREADY   = 2,    /**< libsir is already initialized */
    SIR_E_DUPFILE   = 3,    /**< File already managed by libsir */
    SIR_E_NOFILE    = 4,    /**< File not managed by libsir */
    SIR_E_FCFULL    = 5,    /**< Maximum number of files already managed */
    SIR_E_OPTIONS   = 6,    /**< Option flags are invalid */
    SIR_E_LEVELS    = 7,    /**< Level flags are invalid */
    SIR_E_TEXTSTYLE = 8,    /**< Text style is invalid */
    SIR_E_STRING    = 9,    /**< Invalid string argument */
    SIR_E_NODEST    = 10,   /**< No destinations registered for level */
    SIR_E_PLATFORM  = 11,   /**< Platform error code %%d: %%s */
    SIR_E_UNKNOWN   = 4095, /**< Error is not known */    
};

/** @} */

#define _SIR_E_NOERROR   _sir_mkerror(SIR_E_NOERROR)
#define _SIR_E_NOTREADY  _sir_mkerror(SIR_E_NOTREADY)
#define _SIR_E_ALREADY   _sir_mkerror(SIR_E_ALREADY)
#define _SIR_E_DUPFILE   _sir_mkerror(SIR_E_DUPFILE)
#define _SIR_E_NOFILE    _sir_mkerror(SIR_E_NOFILE)
#define _SIR_E_FCFULL    _sir_mkerror(SIR_E_FCFULL)
#define _SIR_E_OPTIONS   _sir_mkerror(SIR_E_OPTIONS)
#define _SIR_E_LEVELS    _sir_mkerror(SIR_E_LEVELS)
#define _SIR_E_TEXTSTYLE _sir_mkerror(SIR_E_TEXTSTYLE)
#define _SIR_E_STRING    _sir_mkerror(SIR_E_STRING)
#define _SIR_E_NODEST    _sir_mkerror(SIR_E_NODEST)
#define _SIR_E_PLATFORM  _sir_mkerror(SIR_E_PLATFORM)
#define _SIR_E_UNKNOWN   _sir_mkerror(SIR_E_UNKNOWN)

static const struct {
    sirerror_t       e;
    const sirchar_t* msg;
} sir_errors[] = {
    { _SIR_E_NOERROR, "The operation completed successfully" },
    { _SIR_E_NOTREADY, "libsir has not been initialized" },
    { _SIR_E_ALREADY, "libsir is already initialized" },
    { _SIR_E_DUPFILE, "File already managed by SIR" },
    { _SIR_E_NOFILE, "File not managed by SIR" },
    { _SIR_E_FCFULL, "Maximum number of files already managed" },
    { _SIR_E_OPTIONS, "Option flags are invalid" },
    { _SIR_E_LEVELS, "Level flags are invalid" },
    { _SIR_E_TEXTSTYLE, "Text style is invalid" },
    { _SIR_E_STRING, "Invalid string argument" },
    { _SIR_E_NODEST, "No destinations registered for level" },
    { _SIR_E_PLATFORM, "Platform error code %d: %s" },
    { _SIR_E_UNKNOWN, "Error is not known"},
};

void __sir_seterror(sirerror_t err, const sirchar_t* func, const sirchar_t* file, uint32_t line);
#define _sir_seterror(err) __sir_seterror(err, __func__, __FILE__, __LINE__)

void __sir_setoserror(int, const sirchar_t*, const sirchar_t*, const sirchar_t*, uint32_t);
#define _sir_setoserror(err, msg) __sir_setoserror(err, msg, __func__, __FILE__, __LINE__)

/** Handle a C library error. */
void __sir_handleerr(int code, const sirchar_t* func, const sirchar_t* file, uint32_t line);
#define _sir_handleerr(code) __sir_handleerr(code, __func__, __FILE__, __LINE__)

#ifdef _WIN32
/**
 * Some Win32 API error codes overlap C library error codes, so they need to be handled separately.
 * Mapping them sounds great, but in practice valuable information about what went wrong is totally
 * lost in translation.
 */
void __sir_handlewin32err(DWORD code, const sirchar_t* func, const sirchar_t* file, uint32_t line);
#define _sir_handlewin32err(code) __sir_handlewin32err(code, __func__, __FILE__, __LINE__)
#endif

/** Returns information about the last error that occurred. */
sirerror_t _sir_geterror(sirchar_t message[SIR_MAXERROR]);

#ifdef SIR_SELFLOG
/** Log an internal message to \a stderr. */
void _sir_selflog(const sirchar_t* format, ...);
#else
#define _sir_selflog(format, ...) ((void)(0))
#endif

#endif /* !_SIR_ERRORS_H_INCLUDED */
