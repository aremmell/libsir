/**
 * @file sirmacros.h
 * @brief Internally used macros and inline functions.
 */
#ifndef _SIR_MACROS_H_INCLUDED
#define _SIR_MACROS_H_INCLUDED

#include "sirtypes.h"

/**
 * @addtogroup intern
 * @{
 */

#define _COUNTOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define _SIR_L_START(format) \
    bool    r = false;       \
    va_list args;            \
    va_start(args, format);

#define _SIR_L_END(args) va_end(args);

/** Validates a string pointer */
static inline bool validstr(const sirchar_t* str) {
    return str && *str;
}

/** Validates a log file identifier. */
static inline bool validid(int id) {
    return id >= 0;
}

/** Validates a set of ::sir_level flags. */
static inline bool validlevels(sir_levels levels) {
    return levels <= SIRL_ALL;
}

/** Validates a single ::sir_level. */
static inline bool validlevel(sir_level level) {
    return 0 != level && !(level & (level - 1));
}

/** Validates a set of ::sir_option flags. */
static inline bool validopts(sir_options opts) {
    return (opts & SIRL_ALL) == 0 && opts <= SIRO_MSGONLY;
}

/** Checks a bitfield for a specific set of bits. */
static inline bool flagtest(uint32_t flags, uint32_t test) {
    return (flags & test) == test;
}

/** Wraps \a free. */
static inline void safefree(void* p) {
    if (!p)
        return;
    free(p);
    p = NULL;
}

/** Wraps \a fclose. */
static inline void safefclose(FILE* f) {
    if (!f)
        return;
    fclose(f);
    f = NULL;
}

/** Places a null terminator at the first index in a string buffer. */
static inline void resetstr(sirchar_t* str) {
    str[0] = (sirchar_t)'\0';
}

/** @} */

#endif /* !_SIR_MACROS_H_INCLUDED */
