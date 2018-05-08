/**
 * @file sirhelpers.h
 * @brief Internally used macros and inline functions.
 */
#ifndef _SIR_MACROS_H_INCLUDED
#define _SIR_MACROS_H_INCLUDED

#include "sirtypes.h"

/**
 * @addtogroup intern
 * @{
 */

/** Computes the size of an array. */
#define _sir_countof(arr) (sizeof(arr) / sizeof(arr[0]))

/**
 * Creates an error code that (hopefully) doesn't conflict
 * with any of those defined by the platform.
 */
#define _sir_mkerror(code) \
    (((uint32_t)(code & 0x7fff) << 16) | 0x80000000)

/** Validates an internal error. */
static inline
bool _sir_validerror(sirerror_t err) {
    sirerror_t masked = err & 0x8fffffff;
    return masked >= 0x80010000 && masked <= 0x8fff0000;
}

/** Extracts just the code from an internal error. */
static inline
uint16_t _sir_geterrcode(sirerror_t err) {
    return (err >> 16) & 0x7fff;
}

/** Evil macro used for _sir_lv wrappers. */
#define _SIR_L_START(format) \
    bool    r = false;       \
    va_list args;            \
    va_start(args, format);

/** Evil macro used for _sir_lv wrappers. */
#define _SIR_L_END(args) va_end(args);

/** Validates a pointer. */
#define _sir_validptr(p) (NULL != p)

/** Checks a bitfield for a specific set of bits. */
static inline
bool _sir_bittest(uint32_t flags, uint32_t test) {
    return (flags & test) == test;
}

/** Wraps \a free. */
static inline
void __sir_safefree(void** p) {
    if (!p || (p && !*p))
        return;
    free(*p);
    *p = NULL;
}

/** Wraps \a free. */
static inline
void _sir_safefree(void* p) {
    __sir_safefree(&p);
}

/** Validates a log file identifier. */
bool _sir_validfid(int id);

/** Validates a set of ::sir_level flags. */
bool _sir_validlevels(sir_levels levels);

/** Validates a single ::sir_level. */
bool _sir_validlevel(sir_level level);

/** Validates a set of ::sir_option flags. */
bool _sir_validopts(sir_options opts);

/** Validates a string pointer and optionally fails if it's invalid. */
bool __sir_validstr(const sirchar_t* str, bool fail);

/** Validates a string pointer and fails if it's invalid. */
static inline
bool _sir_validstr(const sirchar_t* str) {
    return __sir_validstr(str, true);
}

/** Validates a string pointer but ignores if it's invalid. */
static inline
bool _sir_validstrnofail(const sirchar_t* str) {
    return __sir_validstr(str, false);
}

/** Places a null terminator at the first index in a string buffer. */
static inline
void _sir_resetstr(sirchar_t* str) {
    str[0] = (sirchar_t)'\0';
}

/** @} */

#endif /* !_SIR_MACROS_H_INCLUDED */
