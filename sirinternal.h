/**
 * @file sirinternal.h
 * @brief Internal implementation.
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
bool _sir_sanity(void);

/** Validates the configuration passed to ::sir_init. */
bool _sir_options_sanity(const sirinit* si);

/** Initializes libsir. */
bool _sir_init(sirinit* si);

/** Updates levels for \a stdout. */
void _sir_stdoutlevels(sirinit* si, sir_update_data* data);

/** Updates options for \a stdout. */
void _sir_stdoutopts(sirinit* si, sir_update_data* data);

/** Updates levels for \a stderr. */
void _sir_stderrlevels(sirinit* si, sir_update_data* data);

/** Updates options for \a stderr. */
void _sir_stderropts(sirinit* si, sir_update_data* data);

/** Updates levels for \a syslog. */
void _sir_sysloglevels(sirinit* si, sir_update_data* data);

/** Updates levels/options in the global init structure. */
typedef void (*sirinit_update)(sirinit*, sir_update_data*);

/** Updates levels/options in the global init structure. */
bool _sir_writeinit(sir_update_data* data, sirinit_update update);

/** Locks a protected section. */
void* _sir_locksection(sir_mutex_id mid);

/** Unlocks a protected section. */
bool _sir_unlocksection(sir_mutex_id mid);

/** Maps a ::sir_mutex_id to a ::sirmutex_t and protected section. */
bool _sir_mapmutexid(sir_mutex_id mid, sirmutex_t** m, void** section);

/** Frees allocated resources. */
bool _sir_cleanup(void);

#ifndef _WIN32
/** Initializes a specific mutex. */
void _sir_initmutex_si_once(void);
/** Initializes a specific mutex. */
void _sir_initmutex_fc_once(void);
/** Initializes a specific mutex. */
void _sir_initmutex_ts_once(void);
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

/** Retrieves the current local time w/ optional milliseconds. */
bool _sir_getlocaltime(time_t* tbuf, long* nsecbuf);

/** Formats the current time as a string. */
bool _sir_formattime(time_t now, sirchar_t* buffer, const sirchar_t* format);

/** Returns the current process identifier. */
pid_t _sir_getpid(void);

/** Returns the current thread identifier. */
pid_t _sir_gettid(void);

/** Returns the current thread's name. */
bool _sir_getthreadname(char name[SIR_MAXPID]);

/** @} */

#endif /* !_SIR_INTERNAL_H_INCLUDED */
