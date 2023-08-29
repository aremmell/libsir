/*
 * helpers.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.3
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
#ifndef _SIR_HELPERS_H_INCLUDED
# define _SIR_HELPERS_H_INCLUDED

# include "sir/types.h"

/** Computes the size of an array. */
# define _sir_countof(arr) (sizeof(arr) / sizeof(arr[0]))

/** Creates an error code that (hopefully) doesn't conflict with any of those
 * defined by the platform. */
# define _sir_mkerror(code) (((uint32_t)((code) & 0x7fff) << 16) | 0x80000000)

/** Validates an internal error. */
static inline
bool _sir_validerror(uint32_t err) {
    uint32_t masked = err & 0x8fffffff;
    return masked >= 0x80000000 && masked <= 0x8fff0000;
}

/** Extracts just the code from an internal error. */
static inline
uint16_t _sir_geterrcode(uint32_t err) {
    return (err >> 16) & 0x7fff;
}

/** Evil macro used for _sir_lv wrappers. */
# define _SIR_L_START(format) \
    bool ret     = false; \
    va_list args = {0}; \
    do { \
        if (!_sir_validptr(format)) \
            return false; \
        va_start(args, format); \
    } while (false)

/** Evil macro used for _sir_lv wrappers. */
# define _SIR_L_END() \
    va_end(args)

/** Evil macros used to enter/leave locked sections. */
# define _SIR_LOCK_SECTION(type, name, mid, ret) \
    type* name = _sir_locksection(mid); \
    do { \
        if (!name) { \
            (void)_sir_seterror(_SIR_E_INTERNAL); \
            return ret; \
        } \
    } while (false)

/** Evil macros used to enter/leave locked sections. */
# define _SIR_UNLOCK_SECTION(mid) \
    _sir_unlocksection(mid)

/** Squelches warnings about unreferenced parameters. */
# define SIR_UNUSED(param) (void)param

/** Combines SIR_ASSERT and SIR_UNUSED, which are frequently used together. */
# define SIR_ASSERT_UNUSED(assertion, var) SIR_ASSERT(assertion); SIR_UNUSED(var)

/** Returns a printable string even if NULL. */
# define _SIR_PRNSTR(str) (str ? str : "<null>")

/** Even more evil macros used for binary searching arrays. */
# define _SIR_DECLARE_BIN_SEARCH(low, high) \
    size_t _low = low, _high = high; \
    size_t _mid = (_low + _high) / 2

# define _SIR_BEGIN_BIN_SEARCH() do {

# define _SIR_ITERATE_BIN_SEARCH(comparison) \
    if (_low == _high) \
        break; \
    if (0 > comparison && (_mid - 1) >= _low) { \
        _high = _mid - 1; \
    } else if ((_mid + 1) <= _high) { \
        _low = _mid + 1; \
    } else { \
        break; \
    } \
    _mid = (_low + _high) / 2

# define _SIR_END_BIN_SEARCH() \
    } while (true)

/* Validates a pointer and optionally fails if it's invalid. */
bool __sir_validptr(const void* restrict p, bool fail);

/** Validates a pointer-to-pointer and optionally fails if it's invalid. */
bool __sir_validptrptr(const void* restrict* pp, bool fail);

/** Validates a pointer but ignores whether it's invalid. */
# define _sir_validptrnofail(p) __sir_validptr(p, false)

/** Validates a pointer and fails if it's invalid. */
# define _sir_validptr(p) __sir_validptr((const void* restrict)p, true)

/** Validates a pointer-to-function and fails if it's invalid. */
# define _sir_validfnptr(fnp) __sir_validptrptr((const void* restrict*)&fnp, true)

/** Validates a pointer-to-pointer and fails if it's invalid. */
# define _sir_validptrptr(pp) __sir_validptrptr((const void* restrict*)pp, true)

/** Checks a bitmask for a specific set of bits. */
static inline
bool _sir_bittest(uint32_t flags, uint32_t test) {
    return (flags & test) == test;
}

/** Sets a specific set of bits high in a bitmask. */
static inline
bool _sir_setbitshigh(uint32_t* flags, uint32_t set) {
    if (!flags)
        return false;

    *flags |= set;
    return true;
}

/** Sets a specific set of bits low in a bitmask. */
static inline
bool _sir_setbitslow(uint32_t* flags, uint32_t set) {
    if (!flags)
        return false;

    *flags &= ~set;
    return true;
}

/** Calls free and sets the pointer to NULL. */
void __sir_safefree(void** pp);

/** Wraps __sir_safefree with a cast to void**. */
# define _sir_safefree(pp) __sir_safefree((void**)pp)

/** Calls close and sets the descriptor to -1. */
void _sir_safeclose(int* restrict fd);

/** Calls fclose and sets the stream pointer to NULL. */
void _sir_safefclose(FILE* restrict* restrict f);

/** Validates a log file descriptor. */
bool _sir_validfd(int fd);

/** Validates a file identifier */
static inline
bool _sir_validfileid(sirfileid id) {
    return 0 != id;
}

/** Validates a plugin identifier */
static inline
bool _sir_validpluginid(sirpluginid id) {
    return 0 != id;
}

/** Validates a sir_update_config_data structure. */
bool _sir_validupdatedata(sir_update_config_data* data);

/** Validates a set of ::sir_level flags. */
bool _sir_validlevels(sir_levels levels);

/** Validates a single ::sir_level. */
bool _sir_validlevel(sir_level level);

/** Applies default ::sir_level flags if applicable. */
static inline
void _sir_defaultlevels(sir_levels* levels, sir_levels def) {
    if (levels && SIRL_DEFAULT == *levels)
        *levels = def;
}

/** Applies default ::sir_options flags if applicable. */
static inline
void _sir_defaultopts(sir_options* opts, sir_options def) {
    if (opts && SIRO_DEFAULT == *opts)
        *opts = def;
}

/** Validates a set of ::sir_option flags. */
bool _sir_validopts(sir_options opts);

/** Validates a ::sir_textattr. */
bool _sir_validtextattr(sir_textattr attr);

/** Validates a ::sir_textcolor based on color mode. */
bool _sir_validtextcolor(sir_colormode mode, sir_textcolor color);

/** Validates a ::sir_colormode. */
bool _sir_validcolormode(sir_colormode mode);

/** Converts a SIRTC_* value to a 16-color mode ANSI foreground color. */
static inline
sir_textcolor _sir_mkansifgcolor(sir_textcolor fg) {
    return SIRTC_DEFAULT == fg ? 39 : fg < 8 ? fg + 30 : fg + 82;
}

/** Converts a SIRTC_* value to a 16-color mode ANSI background color. */
static inline
sir_textcolor _sir_mkansibgcolor(sir_textcolor bg) {
    return SIRTC_DEFAULT == bg ? 49 : bg < 8 ? bg + 40 : bg + 92;
}

/** Returns the appropriate ANSI command for the specified foreground color. */
static inline
sir_textcolor _sir_getansifgcmd(sir_textcolor fg) {
    return SIRTC_DEFAULT == fg ? 39 : 38;
}

/** Returns the appropriate ANSI command for the specified background color. */
static inline
sir_textcolor _sir_getansibgcmd(sir_textcolor bg) {
    return SIRTC_DEFAULT == bg ? 49 : 48;
}

/** Extracts the red component out of an RGB color mode ::sir_textcolor. */
# define _sir_getredfromcolor(color) (uint8_t)(((color) >> 16) & 0x000000ff)

/** Sets the red component in an RGB color mode ::sir_textcolor. */
# define _sir_setredincolor(color, red) (color |= (((red) << 16) & 0x00ff0000))

/** Extracts the green component out of an RGB color mode ::sir_textcolor. */
# define _sir_getgreenfromcolor(color) (uint8_t)(((color) >> 8) & 0x000000ff)

/** Sets the green component in an RGB color mode ::sir_textcolor. */
# define _sir_setgreenincolor(color, green) ((color) |= (((green) << 8) & 0x0000ff00))

/** Extracts the blue component out of an RGB color mode ::sir_textcolor. */
# define _sir_getbluefromcolor(color) (uint8_t)((color) & 0x000000ff)

/** Sets the blue component in an RGB color mode ::sir_textcolor. */
# define _sir_setblueincolor(color, blue) ((color) |= ((blue) & 0x000000ff))

/** Sets the red, blue, and green components in an RGB color mode ::sir_textcolor. */
static inline
sir_textcolor _sir_makergb(sir_textcolor r, sir_textcolor g, sir_textcolor b) {
    sir_textcolor retval = 0;
    _sir_setredincolor(retval, r);
    _sir_setgreenincolor(retval, g);
    _sir_setblueincolor(retval, b);
    return retval;
}

/** Validates a string pointer and optionally fails if it's invalid. */
bool __sir_validstr(const char* restrict str, bool fail);

/** Validates a string pointer and fails if it's invalid. */
# define _sir_validstr(str) __sir_validstr(str, true)

/** Validates a string pointer but ignores whether it's invalid. */
# define _sir_validstrnofail(str) __sir_validstr(str, false)

/** Places a null terminator at the first index in a string buffer. */
static inline
void _sir_resetstr(char* str) {
    str[0] = '\0';
}

/**
 * Wrapper for strncmp. Just easier to read and use if you only wish to
 * test for equality. Not case-sensitive.
 */
static inline
bool _sir_strsame(const char* lhs, const char* rhs, size_t count) {
    return 0 == strncmp(lhs, rhs, count);
}

/**
 * Wrapper for str[n,l]cpy/strncpy_s. Determines which one to use
 * based on preprocessor macros.
 */
int _sir_strncpy(char* restrict dest, size_t destsz,
    const char* restrict src, size_t count);

/**
 * Wrapper for str[n,l]cat/strncat_s. Determines which one to use
 * based on preprocessor macros.
 */
int _sir_strncat(char* restrict dest, size_t destsz,
    const char* restrict src, size_t count);

/**
 * Wrapper for fopen/fopen_s. Determines which one to use
 * based on preprocessor macros.
 */
int _sir_fopen(FILE* restrict* restrict streamptr, const char* restrict filename,
    const char* restrict mode);

/**
 * Wrapper for localtime/localtime_s. Determines which one to use
 * based on preprocessor macros.
 */
struct tm* _sir_localtime(const time_t* restrict timer, struct tm* restrict buf);

/**
 * A portable "press any key to continue" implementation; On Windows, uses _getch().
 * otherwise, uses tcgetattr()/tcsetattr() and getchar().
 */
bool _sir_getchar(char* input);

/**
 * Implementation of the 32-bit FNV-1a OWHF (http://isthe.com/chongo/tech/comp/fnv/)
 */
static inline
uint32_t FNV32_1a(const uint8_t* data, size_t len) {
    uint32_t hash = 2166136261U;
    for (size_t n = 0; n < len; n++) {
        hash ^= (uint32_t)data[n];
        hash = (uint32_t)(hash * 16777619ULL);
    }
    return hash;
}

/**
 * Wrapper for snprintf when truncation is intended.
 */
# define _sir_snprintf_trunc(dst, size, ...) \
    do { \
      volatile size_t _n = size; \
      (void)snprintf(dst, _n, __VA_ARGS__); \
    } while (false)

/**
 * Implementation of the 64-bit FNV-1a OWHF (http://isthe.com/chongo/tech/comp/fnv/)
 * watered down to only handle null-terminated strings.
 */
# if defined(__clang__) /* only Clang has unsigned-integer-overflow; GCC BZ#96829 */
SANITIZE_SUPPRESS("unsigned-integer-overflow")
# endif
static inline
uint64_t FNV64_1a(const char* str)
{
    uint64_t hash = 14695981039346656037ULL;
    for (const char* c = str; *c; c++) {
        hash ^= (uint64_t)(unsigned char)(*c);
        hash *= 1099511628211ULL; /* unsigned-integer-overflow */
    }
    return hash;
}

#endif /* !_SIR_HELPERS_H_INCLUDED */
