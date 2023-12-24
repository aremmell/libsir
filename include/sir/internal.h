/*
 * internal.h
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
 * Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
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
 *
 * -----------------------------------------------------------------------------
 */

#ifndef _SIR_INTERNAL_H_INCLUDED
# define _SIR_INTERNAL_H_INCLUDED

# include "sir/helpers.h"
# include "sir/maps.h"
# include "sir/errors.h"

# if defined(__cplusplus)
extern "C" {
# endif

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
bool _sir_isinitialized(void);

/**
 * Evaluates whether or not libsir has been initialized, and sets the last
 * error to SIR_E_NOTREADY if not initialized.
 */
bool _sir_sanity(void);

/** Validates the configuration passed to ::sir_init. */
bool _sir_init_sanity(const sirinit* si);

/** Resets TLS data. */
void _sir_reset_tls(void);

/** Updates levels for stdout. */
bool _sir_stdoutlevels(sirinit* si, const sir_update_config_data* data);

/** Updates options for stdout. */
bool _sir_stdoutopts(sirinit* si, const sir_update_config_data* data);

/** Updates levels for stderr. */
bool _sir_stderrlevels(sirinit* si, const sir_update_config_data* data);

/** Updates options for stderr. */
bool _sir_stderropts(sirinit* si, const sir_update_config_data* data);

/** Updates levels for the system logger. */
bool _sir_sysloglevels(sirinit* si, const sir_update_config_data* data);

/** Updates options for the system logger. */
bool _sir_syslogopts(sirinit* si, const sir_update_config_data* data);

/** Updates the identity for the system logger.*/
bool _sir_syslogid(sirinit* si, const sir_update_config_data* data);

/** Updates the category for the system logger. */
bool _sir_syslogcat(sirinit* si, const sir_update_config_data* data);

/** Callback for updating values in the global config. */
typedef bool (*sirinit_update)(sirinit*, const sir_update_config_data*);

/** Updates values in the global config. */
bool _sir_writeinit(const sir_update_config_data* data, sirinit_update update);

/** Locks a protected section. */
void* _sir_locksection(sir_mutex_id mid);

/** Unlocks a protected section. */
void _sir_unlocksection(sir_mutex_id mid);

/** Maps a ::sir_mutex_id to a ::sir_mutex and protected section. */
bool _sir_mapmutexid(sir_mutex_id mid, sir_mutex** m, void** section);

# if !defined(__WIN__)
/** Static initialization procedure. */
void _sir_init_static_once(void);
# else /* __WIN__ */
/** Static initialization procedure. */
BOOL CALLBACK _sir_init_static_once(PINIT_ONCE ponce, PVOID param, PVOID* ctx);
# endif

/** Shared one-time static data initialization routine. */
bool _sir_init_common_static(void);

/** Executes only one time. */
bool _sir_once(sir_once* once, sir_once_fn func);

/** Core output formatting. */
PRINTF_FORMAT_ATTR(2, 0)
bool _sir_logv(sir_level level, PRINTF_FORMAT const char* format, va_list args);

/** Output dispatching. */
bool _sir_dispatch(const sirinit* si, sir_level level, sirbuf* buf);

/** Specific destination formatting. */
const char* _sir_format(bool styling, sir_options opts, sirbuf* buf);

/** Initializes a ::sir_syslog_dest. */
bool _sir_syslog_init(const char* name, sir_syslog_dest* ctx);

/**
 * Abstraction for setup of platform-specific implementations of system
 * logger facilities.
 *
 * Called upon initialization of the library (and if the configuration is modified).
 * Performs any necessary preparation: connecting/opening handles, etc.
 */
bool _sir_syslog_open(sir_syslog_dest* ctx);

/**
 * Abstraction for writing to platform-specific implementations of
 * system logger facilities.
 */
bool _sir_syslog_write(sir_level level, const sirbuf* buf, const sir_syslog_dest* ctx);

/**
 * Called after updates to the global config that may require reconfiguration
 * of the system logger.
 */
bool _sir_syslog_updated(sirinit* si, const sir_update_config_data* data);

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
bool _sir_clock_gettime(int clock, time_t* tbuf, long* msecbuf);

/**
 * Returns the number of milliseconds elapsed since a point in time represented
 * by the when parameter.
 */
double _sir_msec_since(const sir_time* when, sir_time* out);

/** Returns the current process identifier. */
pid_t _sir_getpid(void);

/** Returns the current thread identifier. */
pid_t _sir_gettid(void);

/** Retrieves the current thread's name. */
bool _sir_getthreadname(char name[SIR_MAXPID]);

/** Sets the current thread's name. */
bool _sir_setthreadname(const char* name);

/** Retrieves the hostname of this machine. */
bool _sir_gethostname(char name[SIR_MAXHOST]);

/** Retrieves the number of available logical processors on this machine. */
long __sir_nprocs(bool test_mode);
# define _sir_nprocs() __sir_nprocs(0)
# define _sir_nprocs_test() __sir_nprocs(1)

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_INTERNAL_H_INCLUDED */
