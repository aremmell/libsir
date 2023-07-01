/*
 * sirplatform.h
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
#ifndef _SIR_PLATFORM_H_INCLUDED
#define _SIR_PLATFORM_H_INCLUDED

#if !defined(_WIN32)
# if defined(__STDC_NO_ATOMICS__)
#  undef __HAVE_ATOMIC_H__
# else
#  define __HAVE_ATOMIC_H__
# endif
# define __STDC_WANT_LIB_EXT1__ 1
# if defined(__APPLE__) && defined(__MACH__)
#  define __MACOS__
#  define _DARWIN_C_SOURCE
# elif defined(__FreeBSD__)
#  define __BSD__
#  define _BSD_SOURCE
#  if !defined(_DEFAULT_SOURCE)
#   define _DEFAULT_SOURCE
#  endif
#  if __FreeBSD_version >= 1202500
#   define __FreeBSD_PTHREAD_NP_12_2__
#  elif __FreeBSD_version >= 1103500
#   define __FreeBSD_PTHREAD_NP_11_3__
#  endif
# else
#  if defined(__linux__) && !defined(_GNU_SOURCE)
#   define _GNU_SOURCE
#  endif
#  if !defined(_POSIX_C_SOURCE)
#   define _POSIX_C_SOURCE 200809L
#  endif
#  if !defined(_DEFAULT_SOURCE)
#   define _DEFAULT_SOURCE
#  endif
#  if !defined(_XOPEN_SOURCE)
#   define _XOPEN_SOURCE 700
#  endif
# endif
#else /* _WIN32 */
# define SIR_NO_SYSTEM_LOGGERS
# undef __HAVE_ATOMIC_H__
# define __WANT_STDC_SECURE_LIB__ 1
# define _CRT_RAND_S
# define WIN32_LEAN_AND_MEAN
# define WINVER 0x0A00 /** Windows 10 SDK */
# define _WIN32_WINNT 0x0A00
# define __WIN__
# include <windows.h>
# include <io.h>
# include <synchapi.h>
# include <process.h>
# include <winsock2.h>
# include <conio.h>
# include <shlwapi.h>
# include <direct.h>
#endif

#if defined(SIR_ASSERT_ENABLED)
# include <assert.h>
#define SIR_ASSERT(...) assert(__VA_ARGS__)
#else
# define SIR_ASSERT(...) \
     if (!(__VA_ARGS__)) { \
         _sir_selflog(LRED("!!! would be asserting: " #__VA_ARGS__ "")); \
     }
#endif

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <time.h>

#if !defined(SIR_NO_SYSTEM_LOGGERS)
# if defined(__MACOS__)
#  define SIR_OS_LOG_ENABLED
# else
#  undef SIR_OS_LOG_ENABLED
#  define SIR_SYSLOG_ENABLED
# endif
#else
# undef SIR_OS_LOG_ENABLED
# undef SIR_SYSLOG_ENABLED
#endif

#define SIR_MAXHOST 256

#if defined(__GLIBC__)
# if (__GLIBC__ >= 2 && __GLIBC_MINOR__ > 19) || \
     ((__GLIBC__ == 2 && __GLIBC_MINOR__ <= 19) && defined(_BSD_SOURCE))
#  define __HAVE_UNISTD_READLINK__
# endif
#endif

#if !defined(__WIN__)
# include <pthread.h>
# include <unistd.h>
# include <sys/syscall.h>
# include <sys/time.h>
# include <strings.h>
# include <termios.h>
# include <limits.h>
# include <fcntl.h>
# include <libgen.h>
# include <stdatomic.h>
# if defined(SIR_SYSLOG_ENABLED)
#  include <syslog.h>
# endif
# if defined(__BSD__)
#  include <pthread_np.h>
#  include <sys/sysctl.h>
# elif defined(__linux__)
#  include <linux/limits.h>
# elif defined(__MACOS__)
#  include <mach-o/dyld.h>
#  include <sys/_types/_timespec.h>
#  include <mach/mach.h>
#  include <mach/clock.h>
#  include <mach/mach_time.h>
#  if defined(SIR_OS_LOG_ENABLED)
#   include <os/log.h>
#   include <os/trace.h>
#   include <os/activity.h>
#  endif
# endif

# if defined(PATH_MAX)
#  define SIR_MAXPATH PATH_MAX
# elif defined(MAXPATHLEN)
#  define SIR_MAXPATH MAXPATHLEN
# else
#  define SIR_MAXPATH 1024
# endif

# if defined(__MACOS__)
#  define SIR_MSEC_TIMER
#  define SIR_MSEC_MACH
# elif _POSIX_TIMERS > 0
#  define SIR_MSEC_TIMER
#  define SIR_MSEC_POSIX
# else
#  undef SIR_MSEC_TIMER
# endif

/** The mutex type. */
typedef pthread_mutex_t sirmutex_t;

/** The one-time type. */
typedef pthread_once_t sironce_t;

/** The one-time execution function type. */
typedef void (*sir_once_fn)(void);

/** The one-time initializer. */
# define SIR_ONCE_INIT PTHREAD_ONCE_INIT

#else /* __WIN__ */

# define SIR_MAXPATH MAX_PATH

# define SIR_MSEC_TIMER
# define SIR_MSEC_WIN32

/** The mutex type. */
typedef HANDLE sirmutex_t;

/** The one-time type. */
typedef INIT_ONCE sironce_t;

/** Process/thread ID. */
typedef int pid_t;

/** The one-time execution function type. */
typedef BOOL(CALLBACK* sir_once_fn)(PINIT_ONCE, PVOID, PVOID*);

/** The one-time initializer. */
# define SIR_ONCE_INIT INIT_ONCE_STATIC_INIT

#endif // !__WIN__

#if !defined(thread_local)
# if __STDC_VERSION__ >= 201112 && !defined(__STDC_NO_THREADS__)
#  define thread_local _Thread_local
# elif defined(__WIN__)
#  define thread_local __declspec(thread)
# elif defined(__GNUC__)
#  define thread_local __thread
# else
#  error "unable to resolve thread local attribute; please contact the author."
# endif
#endif

#if defined(__WIN__) && defined(__STDC_SECURE_LIB__)
# define __HAVE_STDC_SECURE_OR_EXT1__
#elif defined(__STDC_LIB_EXT1__)
# define __HAVE_STDC_SECURE_OR_EXT1__
#endif

#if !defined(__MACOS__)
# if defined(__linux__) && _POSIX_C_SOURCE >= 199309L
#  define SIR_MSECCLOCK CLOCK_REALTIME
# else
#  define SIR_MSECCLOCK CLOCK_REALTIME
# endif
#else /* __MACOS__ */
# define SIR_MSECCLOCK REALTIME_CLOCK
#endif

#if (defined(__clang__) || defined(__GNUC__)) && defined(__FILE_NAME__)
# define __file__ __FILE_NAME__
#elif defined(__BASE_FILE__)
# define __file__ __BASE_FILE__
#else
# define __file__ __FILE__
#endif

#endif /* !_SIR_PLATFORM_H_INCLUDED */
