/*
 * sirinternal.h
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.0
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
#ifndef _SIR_INTERNAL_H_INCLUDED
#define _SIR_INTERNAL_H_INCLUDED

#include "sirhelpers.h"
#include "sirmaps.h"
#include "sirerrors.h"

/**
 * Initializes a ::sirinit structure suitable to pass to ::sir_init
 * without modification.
 */
bool _sir_makeinit(sirinit* si);

/** Initializes libsir. */
bool _sir_init(sirinit* si);

/** Un-initializes libsir. */
bool _sir_cleanup(void);

/** Evaluates whether or not libsir has been initialized. */
bool _sir_sanity(void);

/** Validates the configuration passed to ::sir_init. */
bool _sir_init_sanity(const sirinit* si);

/** Updates levels for stdout. */
bool _sir_stdoutlevels(sirinit* si, sir_update_config_data* data);

/** Updates options for stdout. */
bool _sir_stdoutopts(sirinit* si, sir_update_config_data* data);

/** Updates levels for stderr. */
bool _sir_stderrlevels(sirinit* si, sir_update_config_data* data);

/** Updates options for stderr. */
bool _sir_stderropts(sirinit* si, sir_update_config_data* data);

/** Updates levels for the system logger. */
bool _sir_sysloglevels(sirinit* si, sir_update_config_data* data);

/** Updates options for the system logger. */
bool _sir_syslogopts(sirinit* si, sir_update_config_data* data);

/** Updates the identity for the system logger.*/
bool _sir_syslogid(sirinit* si, sir_update_config_data* data);

/** Updates the category for the system logger. */
bool _sir_syslogcat(sirinit* si, sir_update_config_data* data);

/** Callback for updating values in the global config. */
typedef bool (*sirinit_update)(sirinit*, sir_update_config_data*);

/** Updates values in the global config. */
bool _sir_writeinit(sir_update_config_data* data, sirinit_update update);

/** Locks a protected section. */
void* _sir_locksection(sir_mutex_id mid);

/** Unlocks a protected section. */
void _sir_unlocksection(sir_mutex_id mid);

/** Maps a ::sir_mutex_id to a ::sirmutex_t and protected section. */
bool _sir_mapmutexid(sir_mutex_id mid, sirmutex_t** m, void** section);

#if !defined(__WIN__)
/** General initialization procedure. */
void _sir_initialize_once(void);
/** Initializes a specific mutex. */
void _sir_initmutex_cfg_once(void);
/** Initializes a specific mutex. */
void _sir_initmutex_fc_once(void);
/** Initializes a specific mutex. */
void _sir_initmutex_ts_once(void);
#else /* __WIN__ */
/** General initialization procedure. */
BOOL CALLBACK _sir_initialize_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
/** Initializes a specific mutex. */
BOOL CALLBACK _sir_initmutex_cfg_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
/** Initializes a specific mutex. */
BOOL CALLBACK _sir_initmutex_fc_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
/** Initializes a specific mutex. */
BOOL CALLBACK _sir_initmutex_ts_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
#endif

/** Executes only one time. */
bool _sir_once(sironce_t* once, sir_once_fn func);

/** Core output formatting. */
bool _sir_logv(sir_level level, const char* format, va_list args);

/** Output dispatching. */
bool _sir_dispatch(sirinit* si, sir_level level, sirbuf* buf);

/** Specific destination formatting. */
const char* _sir_format(bool styling, sir_options opts, sirbuf* buf);

/** Initializes a ::sir_syslog_dest. */
bool _sir_syslog_init(const char* name, sir_syslog_dest* ctx);

/**
 * Abstraction for setup of platform-specific implementations of system
 * logger facilities.
 *
 * Called upon initialization of the library (and if the configuration is modified).
 * Performs any necesssary preparation: connecting/opening handles, etc.
 */
bool _sir_syslog_open(sir_syslog_dest* ctx);

/**
 * Abstraction for writing to platform-specific implementations of
 * system logger facilities.
 */
bool _sir_syslog_write(sir_level level, const sirbuf* buf, sir_syslog_dest* ctx);

/**
 * Called after updates to the global config that may require reconfiguration
 * of the system logger.
 */
bool _sir_syslog_updated(sirinit* si, sir_update_config_data* data);

/**
 * Abstraction for cleanup/closure of platform-specific implementations of
 * system logger facilities.
 *
 * Called upon shutdown of the library (and if the configuration is modified).
 * Performs any necessary operations: disconnecting/closing handles, etc.
 */
bool _sir_syslog_close(sir_syslog_dest* ctx);

/** Resets the internal state. */
void _sir_syslog_reset(sir_syslog_dest* ctx);

/** Returns the formatted, human-readable form of a ::sir_level. */
const char* _sir_formattedlevelstr(sir_level level);

/** Retrieves the current time w/ optional milliseconds. */
bool _sir_clock_gettime(time_t* tbuf, long* msecbuf);

/** Formats the current time as a string. */
bool _sir_formattime(time_t now, char* buffer, const char* format);

/** Returns the current process identifier. */
pid_t _sir_getpid(void);

/** Returns the current thread identifier. */
pid_t _sir_gettid(void);

/** Retrieves the current thread's name. */
bool _sir_getthreadname(char name[SIR_MAXPID]);

/** Retrieves the hostname of this machine. */
bool _sir_gethostname(char name[SIR_MAXHOST]);

#endif /* !_SIR_INTERNAL_H_INCLUDED */
