/*!
 * \file sirplatform.h
 */
#ifndef _SIR_PLATFORM_H_INCLUDED
#define _SIR_PLATFORM_H_INCLUDED

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#else
#define _WIN32_WINNT 0x0600
#endif

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#ifndef _WIN32
#include <pthread.h>
#include <stdatomic.h>
#include <sys/syscall.h>
#include <syslog.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/limits.h>
#endif

#ifdef PATH_MAX
#define SIR_MAXPATH PATH_MAX
#endif

#if _POSIX_TIMERS > 0
#define SIR_MSEC_TIMER
#define SIR_MSEC_POSIX
#else
#undef SIR_MSEC_TIMER
#endif

typedef int             sirerror_t;
typedef pthread_mutex_t sirmutex_t;
typedef pthread_once_t  sironce_t;
typedef void (*sir_once_fn)();

#define SIR_ONCE_INIT PTHREAD_ONCE_INIT
#else
#define WIN32_LEAN_AND_MEAN

#include <io.h>
#include <synchapi.h>
#include <windows.h>

#define SIR_MAXPATH MAX_PATH
#define SIR_NO_SYSLOG
#define SIR_MSEC_TIMER
#define SIR_MSEC_WIN32

typedef DWORD     sirerror_t;
typedef HANDLE    sirmutex_t;
typedef INIT_ONCE sironce_t;
typedef BOOL(CALLBACK* sir_once_fn)(PINIT_ONCE, PVOID, PVOID*);

#define SIR_ONCE_INIT INIT_ONCE_STATIC_INIT
#endif

/*! A value that represents the success condition. */
#define SIR_NOERROR (sirerror_t)0

/*! A sensible (?) constraint for the limit of a file's path. Note that this value
 * is only used in the absence of PATH_MAX. */
#ifndef SIR_MAXPATH
#define SIR_MAXPATH 65535
#endif

#endif /* !_SIR_PLATFORM_H_INCLUDED */
