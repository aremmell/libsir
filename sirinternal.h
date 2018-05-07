/**
 * @file sirinternal.h
 * @brief Core internal definitions.
 */
#ifndef _SIR_INTERNAL_H_INCLUDED
#define _SIR_INTERNAL_H_INCLUDED

#include "sirhelpers.h"
#include "sirtypes.h"
#include "sirerrors.h"

/** 
 * @defgroup intern Internal
 * 
 * Guts of the library.
 * 
 * @addtogroup intern 
 * @{
 */

/** Evaluates whether or not the library has been initialized. */
bool _sir_sanity();

/** Validates the configuration passed to ::sir_init. */
bool _sir_options_sanity(const sirinit* si);

/** Initializes the library. */
bool _sir_init(sirinit* si);

/** Locks a protected section. */
void* _sir_locksection(sir_mutex_id mid);

/** Unlocks a protected section. */
bool _sir_unlocksection(sir_mutex_id mid);

/** Maps a ::sir_mutex_id to a ::sirmutex_t and protected section. */
bool _sir_mapmutexid(sir_mutex_id mid, sirmutex_t** m, void** section);

/** Frees allocated resources. */
bool _sir_cleanup();

#ifndef _WIN32
/** Initializes a specific mutex. */
void _sir_initmutex_si_once();
/** Initializes a specific mutex. */
void _sir_initmutex_fc_once();
/** Initializes a specific mutex. */
void _sir_initmutex_ts_once();
#else
/** Initializes a specific mutex. */
BOOL CALLBACK _sir_initmutex_si_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
/** Initializes a specific mutex. */
BOOL CALLBACK _sir_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
/** Initializes a specific mutex. */
BOOL CALLBACK _sir_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
#endif

/** Initializes a specific mutex. */
void _sir_initmutex(sirmutex_t* mutex);

/** Executes only one time. */
void _sir_once(sironce_t* once, sir_once_fn func);

/** Core output formatting. */
bool _sir_logv(sir_level level, const sirchar_t* format, va_list args);

/** Output dispatching. */
bool _sir_dispatch(sirinit* si, sir_level level, siroutput* output);

/** Specific destination formatting. */
const sirchar_t* _sir_format(bool styling, sir_options opts, siroutput* output);

#ifndef SIR_NO_SYSLOG
/** Maps a ::sir_level to a \a syslog level. */
int _sir_syslog_maplevel(sir_level level);
#endif

/** Retrieves a buffer from a ::sirbuf. */
sirchar_t* _sirbuf_get(sirbuf* buf, size_t idx);

/** Converts a ::sir_level to its human-readable form. */
const sirchar_t* _sir_levelstr(sir_level level);

/** Determines if a particular destination is registered to receive output
 * for a particular ::sir_level. */
bool _sir_destwantslevel(sir_levels destLevels, sir_level level);

/** Retrieves the current local time w/ optional milliseconds. */
bool _sir_getlocaltime(time_t* tbuf, long* nsecbuf);

/** Formats the current time as a string. */
bool _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format);

/** Returns the current process identifier. */
pid_t _sir_getpid();

/** Returns the current thread identifier. */
pid_t _sir_gettid();

/** Validates a string pointer */
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

/** Validates a log file identifier. */
bool _sir_validfid(int id);

/** Validates a set of ::sir_level flags. */
bool _sir_validlevels(sir_levels levels);

/** Validates a single ::sir_level. */
bool _sir_validlevel(sir_level level);

/** Validates a set of ::sir_option flags. */
bool _sir_validopts(sir_options opts);

#define _sir_handleerr(code) \
    _sir_handleerr_impl((sirerror_t)code, __func__, __FILE__, __LINE__);

/** Handle an OS error and send it to \a stderr. */
void _sir_handleerr_impl(sirerror_t code, const sirchar_t* func, const sirchar_t* file, uint32_t line);

#ifdef SIR_SELFLOG
/** Log an internal message to \a stderr. */
void _sir_selflog(const sirchar_t* format, ...);
#else
#define _sir_selflog(format, ...) ((void)(0))
#endif

/** @} */

#endif /* !_SIR_INTERNAL_H_INCLUDED */
