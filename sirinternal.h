/**
 * @file sirinternal.h
 * @brief Core internal definitions.
 */
#ifndef _SIR_INTERNAL_H_INCLUDED
#define _SIR_INTERNAL_H_INCLUDED

#include "sirhelpers.h"
#include "sirerrors.h"

/** 
 * @defgroup intern Internal
 * 
 * libsir's guts.
 * 
 * @addtogroup intern 
 * @{
 */

/** Evaluates whether or not libsir has been initialized. */
bool _sir_sanity();

/** Validates the configuration passed to ::sir_init. */
bool _sir_options_sanity(const sirinit* si);

/** Initializes libsir. */
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

/** @} */

#endif /* !_SIR_INTERNAL_H_INCLUDED */
