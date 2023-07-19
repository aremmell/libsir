/*
 * errors.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.1
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
#ifndef _SIR_ERRORS_H_INCLUDED
# define _SIR_ERRORS_H_INCLUDED

# include "sir/helpers.h"

/**
 * @addtogroup publictypes Types
 * @{
 */

/** Error codes. */
enum sir_errorcode {
    SIR_E_NOERROR   = 0,    /**< The operation completed successfully */
    SIR_E_NOTREADY  = 1,    /**< libsir has not been initialized */
    SIR_E_ALREADY   = 2,    /**< libsir is already initialized */
    SIR_E_DUPITEM   = 3,    /**< Item already managed by libsir */
    SIR_E_NOITEM    = 4,    /**< Item not managed by libsir */
    SIR_E_NOROOM    = 5,    /**< Maximum number of items already stored */
    SIR_E_OPTIONS   = 6,    /**< Option flags are invalid */
    SIR_E_LEVELS    = 7,    /**< Level flags are invalid */
    SIR_E_TEXTSTYLE = 8,    /**< Text style is invalid */
    SIR_E_STRING    = 9,    /**< Invalid string argument */
    SIR_E_NULLPTR   = 10,   /**< NULL pointer argument */
    SIR_E_INVALID   = 11,   /**< Invalid argument */
    SIR_E_NODEST    = 12,   /**< No destinations registered for level */
    SIR_E_UNAVAIL   = 13,   /**< Feature is disabled or unavailable */
    SIR_E_INTERNAL  = 14,   /**< An internal error has occurred */
    SIR_E_COLORMODE = 15,   /**< Color mode is invalid */
    SIR_E_TEXTATTR  = 16,   /**< Text attributes are invalid */
    SIR_E_TEXTCOLOR = 17,   /**< Text color is invalid for mode */
    SIR_E_PLUGINBAD = 18,   /**< Plugin module is malformed */
    SIR_E_PLUGINDAT = 19,   /**< Data produced by plugin is invalid */
    SIR_E_PLUGINVER = 20,   /**< Plugin interface version unsupported */
    SIR_E_PLUGINERR = 21,   /**< Plugin reported failure */
    SIR_E_PLATFORM  = 22,   /**< Platform error code %%d: %%s */
    SIR_E_UNKNOWN   = 4095, /**< Unknown error */
};

/** @} */

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

static const struct {
    uint32_t e;
    const char* msg;
} sir_errors[] = {
    {_SIR_E_NOERROR,   "The operation completed successfully"}, //-V616
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
    {_SIR_E_PLATFORM,  "Platform error code %d: %s"},
    {_SIR_E_UNKNOWN,   "Unknown error"},
};

//-V:_sir_seterror:616
bool __sir_seterror(uint32_t err, const char* func, const char* file, uint32_t line);
# define _sir_seterror(err) __sir_seterror(err, __func__, __file__, __LINE__)

void __sir_setoserror(int code, const char* msg, const char* func,
    const char* file, uint32_t line);

/** Handle a C library error. */
bool __sir_handleerr(int code, const char* func, const char* file, uint32_t line); //-V1071
# define _sir_handleerr(code) __sir_handleerr(code, __func__, __file__, __LINE__)

# if defined(__WIN__)
void _sir_invalidparameter(const wchar_t* expr, const wchar_t* func, const wchar_t* file,
    unsigned int line, uintptr_t reserved);

/**
 * Some Win32 API error codes overlap C library error codes, so they need to be handled separately.
 * Mapping them sounds great, but in practice, valuable information about what went wrong is totally
 * lost in translation.
 */
bool __sir_handlewin32err(DWORD code, const char* func, const char* file, uint32_t line);
#  define _sir_handlewin32err(code) __sir_handlewin32err((DWORD)code, __func__, __file__, __LINE__)
# endif

/** Returns information about the last error that occurred. */
uint32_t _sir_geterror(char message[SIR_MAXERROR]);

# if defined(SIR_SELFLOG)
/** Log an internal debug message to stderr. */
PRINTF_FORMAT_ATTR(4, 5)
void __sir_selflog(const char* func, const char* file, uint32_t line, PRINTF_FORMAT const char* format, ...);
#  define _sir_selflog(...) __sir_selflog(__func__, __file__, __LINE__, __VA_ARGS__)
# else
static inline
void __sir_fakefunc(const char* format, ...) {
    _SIR_UNUSED(format);
}
#  define _sir_selflog(...) __sir_fakefunc(__VA_ARGS__)
# endif

#endif /* !_SIR_ERRORS_H_INCLUDED */
