/*!
 * \file sirmacros.h
 * \brief Internal macros for the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_MACROS_H_INCLUDED
#define _SIR_MACROS_H_INCLUDED

#include "sirplatform.h"
#include "sirtypes.h"

#define _SIR_L_START(format) \
    bool    r = false;       \
    va_list args;            \
    va_start(args, format);

#define _SIR_L_END(args) va_end(args);

#define _sir_handleerr(err) \
    _sir_handleerr_impl(err, __func__, __FILE__, __LINE__);

static inline bool validstr(const sirchar_t* str) {
    return str && *str;
}

static inline bool validid(int id) {
    return id >= 0;
}

static inline bool validlevels(sir_levels levels) {
    return (levels & SIRL_ALL) != 0 && (levels & SIRO_MSGONLY) == 0;
}

static inline bool validopts(sir_options opts) {
    return (opts & SIRL_ALL) == 0 && opts <= SIRO_MSGONLY;
}

static inline bool flagtest(uint32_t flags, uint32_t test) {
    return (flags & test) == test;
}

static inline void safefree(void *p) {
    if (!p) return;
    free(p);
    p = NULL;
}

static inline void safefclose(FILE* f) {
    if (!f) return;
    fclose(f);
    f = NULL;
}

static inline void resetstr(sirchar_t* str) {
    str[0] = (sirchar_t)'\0';
}

#endif /* !_SIR_MACROS_H_INCLUDED */
