/*
 * platform.h
 *
 * Version: 2.2.6
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

#ifndef _SIR_PLATFORM_H_INCLUDED
# define _SIR_PLATFORM_H_INCLUDED

# if defined(_MSC_VER) && !defined(__clang__)
#  include <stddef.h>
#  undef _USE_ATTRIBUTES_FOR_SAL
#  define _USE_ATTRIBUTES_FOR_SAL 1
#  include <sal.h>
#  define PRINTF_FORMAT _Printf_format_string_
#  define PRINTF_FORMAT_ATTR(fmt_p, va_p)
# else
#  define PRINTF_FORMAT
#  if (defined(__MINGW32__) || defined(__MINGW64__)) && \
     !defined(__clang_version__)
#   if !defined(__USE_MINGW_ANSI_STDIO)
#    define __USE_MINGW_ANSI_STDIO 1
#   endif
#   define PRINTF_FORMAT_ATTR(fmt_p, va_p) \
    __attribute__((format (gnu_printf, fmt_p, va_p)))
#  else
#   if !defined(__SUNPRO_C) && !defined(__SUNPRO_CC) && \
      !defined(_CH_) && !defined(__CH__)
#    define PRINTF_FORMAT_ATTR(fmt_p, va_p) \
     __attribute__((format (printf, fmt_p, va_p)))
#   else
#    define PRINTF_FORMAT_ATTR(fmt_p, va_p)
#   endif
#  endif
# endif

# undef HAS_ATTRIBUTE
# if defined __has_attribute && (defined(__clang__) || defined(__GNUC__))
#  define HAS_ATTRIBUTE(atr) __has_attribute(atr)
# else
#  define HAS_ATTRIBUTE(atr) 0
# endif

# undef HAS_INCLUDE
# if defined __has_include
#  define HAS_INCLUDE(inc) __has_include(inc)
# else
#  define HAS_INCLUDE(inc) 0
# endif

# undef HAS_FEATURE
# if defined __has_feature
#  define HAS_FEATURE(fea) __has_feature(fea)
# else
#  define HAS_FEATURE(fea) 0
# endif

# undef SANITIZE_SUPPRESS
# if HAS_ATTRIBUTE(no_sanitize)
#  define SANITIZE_SUPPRESS(str) __attribute__((no_sanitize(str)))
# endif
# if !defined(SANITIZE_SUPPRESS)
#  define SANITIZE_SUPPRESS(str)
# endif

# if HAS_FEATURE(safe_stack) && !defined(SIR_NO_PLUGINS)
#  error "linking DSOs with SafeStack is unsupported; disable SafeStack or enable SIR_NO_PLUGINS"
# endif

# if defined(__OPENOSC_H__)
#  define SIR_IMPL_STRNLEN
# endif

# if !defined(_WIN32)
#  if defined(__STDC_NO_ATOMICS__)
#   undef __HAVE_ATOMIC_H__
#  else
#   define __HAVE_ATOMIC_H__
#  endif
#  if defined(__GNUC__) && __GNUC__ <= 4
#   if defined(__GNUC_MINOR__) && __GNUC_MINOR__ <= 8
#    if !defined(__clang_version__)
#     undef __HAVE_ATOMIC_H__
#    endif
#   endif
#  endif
#  if defined(__DragonFly__)
#   if defined(__clang__) && defined(__clang_version__)
#    undef __HAVE_ATOMIC_H__
#   endif
#  endif
#  if defined(__circle_lang__)
#   undef __HAVE_ATOMIC_H__
#  endif
#  if defined(SUNLINT)
#   undef __HAVE_ATOMIC_H__
#  endif
#  if (defined(__INTEL_COMPILER) && !defined(__llvm__)) || \
     defined(__NVCOMPILER) || defined(__COVERITY__)
#   if !defined(_BITS_FLOATN_H)
#    define _BITS_FLOATN_H
#   endif
#  endif
#  if defined(__QNX__)
#   define _QNX_SOURCE
#  endif
#  if defined(__IMPORTC__)
#   include "sir/platform_importc.h"
#  endif
#  if !defined(__open_xl__) && defined(__ibmxl__) && defined(__ibmxl_release__)
#   if __ibmxl__ <= 16
#    if __ibmxl_release__ <= 1
#     undef __HAVE_ATOMIC_H__
#    endif
#   endif
#  endif
#  if !defined(__open_xl__) && defined(__xlC_ver__) && defined(__IBMC__)
#   if __IBMC__ <= 1610
#    undef __HAVE_ATOMIC_H__
#   endif
#   define __XLC16__ 1
#  endif
#  if !defined(__STDC_WANT_LIB_EXT1__)
#   define __STDC_WANT_LIB_EXT1__ 1
#  endif
#  if !defined(__STDC_WANT_LIB_EXT2__)
#   define __STDC_WANT_LIB_EXT2__ 1
#  endif
#  if defined(__APPLE__) && defined(__MACH__)
#   define __MACOS__
#   undef _DARWIN_C_SOURCE
#   define _DARWIN_C_SOURCE
#  elif defined(__serenity__)
#   define SIR_PTHREAD_GETNAME_NP
#  elif defined(__OpenBSD__)
#   define __BSD__
#   define __FreeBSD_PTHREAD_NP_11_3__
#  elif defined(__NetBSD__)
#   define __BSD__
#   if !defined(_NETBSD_SOURCE)
#    define _NETBSD_SOURCE 1
#   endif
#   define SIR_PTHREAD_GETNAME_NP
#   define __USE_HEX_TIDS__
#  elif defined(__FreeBSD__) || defined(__DragonFly__)
#   define __BSD__
#   define _BSD_SOURCE
#   if !defined(_DEFAULT_SOURCE)
#    define _DEFAULT_SOURCE
#   endif
#   include <sys/param.h>
#   if __FreeBSD_version >= 1202500
#    define __FreeBSD_PTHREAD_NP_12_2__
#   elif __FreeBSD_version >= 1103500
#    define __FreeBSD_PTHREAD_NP_11_3__
#   elif __DragonFly_version >= 400907
#    define __DragonFly_getthreadid__
#   endif
#   if defined(__DragonFly__)
#    define SIR_PTHREAD_GETNAME_NP
#   endif
#  else
#   if defined(__EMSCRIPTEN__)
#    define __USE_GNU
#    define _GNU_SOURCE 1
#    undef SIR_NO_THREAD_NAMES
#    define SIR_NO_THREAD_NAMES
#    include <emscripten.h>
#   endif
#   if defined(__HAIKU__)
#    if !defined(__USE_GNU)
#     define __USE_GNU
#    endif
#    if !defined(_GNU_SOURCE)
#     define _GNU_SOURCE 1
#    endif
#    if defined(__clang__) && !defined(_GNU_PTHREAD_H_)
#     include <pthread.h>
extern /* Workaround a Clang on Haiku bug. */
int pthread_getname_np(pthread_t thread, char* buffer, size_t length);
#    endif
#    define SIR_PTHREAD_GETNAME_NP
#   endif
#   if defined(__GNU__) && !defined(__linux__)
#    undef SIR_NO_THREAD_NAMES
#    define SIR_NO_THREAD_NAMES
#    if !defined(__HURD__)
#     define __HURD__ 1
#    endif
#   endif
#   if defined(__linux__)
#    if !defined(_GNU_SOURCE)
#     define _GNU_SOURCE 1
#    endif
#    include <features.h>
#    if defined(__GLIBC__)
#     undef GLIBC_VERSION
#     define GLIBC_VERSION (((0 + __GLIBC__) * 10000) + ((0 + __GLIBC_MINOR__) * 100))
#    endif
#    if !defined(GLIBC_VERSION)
#     define GLIBC_VERSION 0
#    endif
#    if defined(__GLIBC__) && GLIBC_VERSION >= 21200
#     define SIR_PTHREAD_GETNAME_NP
#    endif
#    if defined(__GLIBC__) && GLIBC_VERSION > 0 && GLIBC_VERSION < 21200
#     undef SIR_NO_THREAD_NAMES
#     define SIR_NO_THREAD_NAMES
#    endif
#    if defined(__GLIBC__) && GLIBC_VERSION > 0 && \
       !defined(__SUNPRO_C) && !defined(__SUNPRO_CC)
#     include <sys/sysinfo.h>
#    endif
#   endif
#   if defined(__CYGWIN__)
#    if !defined(_GNU_SOURCE)
#     define _GNU_SOURCE 1
#    endif
#    define SIR_PTHREAD_GETNAME_NP
#    include <sys/features.h>
#   endif
#   if defined(__ANDROID__) && defined(__ANDROID_API__)
#    if __ANDROID_API__ < 26
#     undef SIR_PTHREAD_GETNAME_NP
#     undef SIR_NO_THREAD_NAMES
#     define SIR_NO_THREAD_NAMES
#    endif
#   endif
#   if defined(__illumos__) || ((defined(__sun) || defined(__sun__)) && \
           (defined(__SVR4) || defined(__svr4__)))
#    define __SOLARIS__
#    define SIR_PTHREAD_GETNAME_NP
#    if !defined(_ATFILE_SOURCE)
#     define _ATFILE_SOURCE 1
#    endif
#    if !defined(__EXTENSIONS__)
#     define __EXTENSIONS__
#    endif
#   endif
#   if !defined(_POSIX_C_SOURCE)
#    define _POSIX_C_SOURCE 200809L
#   endif
#   if !defined(_DEFAULT_SOURCE)
#    define _DEFAULT_SOURCE
#   endif
#   if !defined(_XOPEN_SOURCE)
#    define _XOPEN_SOURCE 700
#   endif
#  endif
# else /* _WIN32 */
#  define __WIN__
#  define __WANT_STDC_SECURE_LIB__ 1
#  define WIN32_LEAN_AND_MEAN
#  undef WINVER
#  define WINVER       0x0A00 /** Windows 10 SDK */
#  undef _WIN32_WINNT
#  define _WIN32_WINNT 0x0A00
#  define _CRT_RAND_S
#  define NOMINMAX
#  if defined(__ORANGEC__)
#   include "sir/platform_orangec.h"
#  endif
#  if defined(__MINGW32__) || defined(__MINGW64__)
#   define SIR_PTHREAD_GETNAME_NP
#  endif
#  include <windows.h>
#  include <io.h>
#  include <synchapi.h>
#  include <processthreadsapi.h>
#  include <process.h>
#  include <winsock2.h>
#  include <conio.h>
#  include <shlwapi.h>
#  include <evntprov.h>
#  undef __HAVE_ATOMIC_H__
#  if defined(_MSC_VER) && _MSC_VER >= 1933 && \
      !defined(__cplusplus) && !defined(__IMPORTC__)
#   include <stdatomic.h>
#   define __HAVE_ATOMIC_H__
#  endif
#  if defined(__MINGW32__) || defined(__MINGW64__)
#   undef __USE_MINGW_ANSI_STDIO
#   define __USE_MINGW_ANSI_STDIO 1
#   include <pthread.h>
typedef /* Workaround a MinGW bug */
void (__cdecl* _invalid_parameter_handler)(
 wchar_t const*, wchar_t const*, wchar_t const*,
 unsigned int, uintptr_t);
_invalid_parameter_handler
_set_thread_local_invalid_parameter_handler(
 _invalid_parameter_handler pNew);
#  endif
#  include "sir/platform_embarcadero.h"
# endif

# if !defined(__MACOS__) && !defined(__BSD__) && !defined(__SOLARIS__) && \
     !defined(__HAIKU__) && !(defined(__GLIBC__) && GLIBC_VERSION >= 23800)
#  define SIR_IMPL_STRLCPY 1
#  define SIR_IMPL_STRLCAT 1
# endif

# if defined(__WIN__)
#  define SIR_IMPL_STRNDUP 1
# endif

# if defined(_AIX)
#  define SIR_IMPL_STRCASESTR 1
# endif

# if defined(__MINGW64__)
#  define PID_CAST (int)
# else
#  define PID_CAST
# endif

# if defined(_AIX) || defined(__CYGWIN__)
#  define CLOCK_CAST (int)
# else
#  define CLOCK_CAST
# endif

# if defined(_AIX)
#  if !defined(_ALL_SOURCE)
#   define _ALL_SOURCE
#  endif
#  if defined(_GNU_SOURCE)
#   undef _GNU_SOURCE
#  endif
#  if !defined(_THREAD_SAFE)
#   define _THREAD_SAFE
#  endif
# endif

# if defined(SIR_ASSERT_ENABLED)
#  include <assert.h>
#  define SIR_ASSERT(...) assert(__VA_ARGS__)
# else
#  if defined(SIR_SELFLOG)
#   define SIR_ASSERT(...) \
     do { \
          if (!(__VA_ARGS__)) { \
               _sir_selflog(SIR_BRED("!!! would be asserting: " #__VA_ARGS__ "")); \
          } \
     } while (false)
#  else
#   define SIR_ASSERT(...)
#  endif
# endif

# if defined(__cplusplus) && !defined(restrict)
#  define restrict //-V1059
# endif

# include <ctype.h>
# include <errno.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stdint.h>
# include <inttypes.h>
# include <stdio.h>

# if defined(__cplusplus) && defined(__NVCOMPILER) && defined(__FLOAT128__)
#  define NVIDIA_FLOAT128 __FLOAT128__
#  undef __FLOAT128__
# endif

# include <stdlib.h>

# if defined(NVIDIA_FLOAT128)
#  define __FLOAT128__ NVIDIA_FLOAT128
#  undef NVIDIA_FLOAT128
# endif

# include <string.h>

# if defined(DUMA)
#  include <duma.h>
# endif

# include <sys/stat.h>
# include <sys/types.h>
# include <limits.h>
# include <time.h>

# if !defined(SIR_NO_SYSTEM_LOGGERS)
#  if defined(__MACOS__) && !defined(__IMPORTC__) && \
      ((defined(__clang__) || defined(__clang_version__)) && \
      !(defined(__INTEL_COMPILER) && !defined(__llvm__)) && \
      defined(__clang_major__) && defined(__clang_minor__) && defined(__clang_patchlevel__))
#   undef SIR_OS_LOG_ENABLED
#   define SIR_OS_LOG_ENABLED
#  elif defined(__WIN__)
#   undef SIR_EVENTLOG_ENABLED
#   if defined(__EMBARCADEROC__) || defined(__ORANGEC__)
#    define SIR_NO_SYSTEM_LOGGERS
#   else
#    define SIR_EVENTLOG_ENABLED
#   endif
#  else
#   undef SIR_OS_LOG_ENABLED
#   define SIR_SYSLOG_ENABLED
#  endif
# else
#  undef SIR_OS_LOG_ENABLED
#  undef SIR_SYSLOG_ENABLED
# endif

# define SIR_MAXHOST 256

# if !defined(__WIN__)
#  if !defined(SIR_NO_PLUGINS)
#   include <dlfcn.h>
#  endif
#  if !defined(_CH_) && !defined(__CH__)
#   include <pthread.h>
#  else
#   include <sched.h>
#   include <ch/pthread.h>
#   undef __HAVE_ATOMIC_H__
#  endif
#  if defined(__illumos__)
#   include <sys/fcntl.h>
#  endif
#  if defined(__linux__) && defined(__USE_GNU) && \
             (defined(__SUNPRO_C) || defined(__SUNPRO_CC))
#   undef __USE_GNU
#   include <fcntl.h>
#   define __USE_GNU
#  else
#   include <fcntl.h>
#  endif
#  include <unistd.h>
#  if defined(__MACOS__)
#   include <sys/sysctl.h>
#  endif
#  if !defined(__CYGWIN__) && !defined(__HAIKU__) && \
      !defined(__serenity__) && !defined(_AIX) && \
      !defined(_CH_) && !defined(__CH__) && !defined(__QNX__) && \
      !defined(__managarm__)
#   include <sys/syscall.h>
#  endif
#  if defined(__QNX__)
#   include <sys/syspage.h>
#  endif
#  include <sys/time.h>
#  include <strings.h>
#  include <termios.h>
#  include <limits.h>
#  include <libgen.h>
#  if defined(__HAVE_ATOMIC_H__) && !defined(__cplusplus)
#   include <stdatomic.h>
#  endif
#  if defined(SIR_SYSLOG_ENABLED)
#   if !defined(__cplusplus)
#    include <syslog.h>
#   endif
#  endif
#  if defined(__CYGWIN__)
#   undef SIR_NO_THREAD_NAMES
#   define SIR_NO_THREAD_NAMES
#  endif
#  if defined(_AIX)
#   include <procinfo.h>
#   include <sys/procfs.h>
#   include <sys/systemcfg.h>
#   undef SIR_NO_THREAD_NAMES
#   define SIR_NO_THREAD_NAMES
#  endif
#  if defined(__BSD__)
#   if !defined(__NetBSD__)
#    include <pthread_np.h>
#   endif
#   include <sys/sysctl.h>
#  elif defined(__linux__)
#   if defined(__GLIBC__)
#    include <linux/limits.h>
#   endif
#  elif defined(__HAIKU__)
#   include <OS.h>
#   include <FindDirectory.h>
#  elif defined(__MACOS__)
#   include <mach-o/dyld.h>
#   if defined(SIR_OS_LOG_ENABLED)
#    include <os/log.h>
#    include <os/trace.h>
#    include <os/activity.h>
#   endif
#  endif

#  if defined(PATH_MAX)
#   define SIR_MAXPATH PATH_MAX
#  elif defined(MAXPATHLEN)
#   define SIR_MAXPATH MAXPATHLEN
#  else
#   define SIR_MAXPATH 1024
#  endif

#  if !defined(SIR_MAXPID) && defined(HAIKU) && defined(B_OS_NAME_LENGTH)
#   define SIR_MAXPID B_OS_NAME_LENGTH
#  endif
#  if !defined(SIR_MAXPID) && defined(PTHREAD_MAX_NAMELEN_NP)
#   define SIR_MAXPID PTHREAD_MAX_NAMELEN_NP
#  endif
#  if !defined(SIR_MAXPID) && defined(_NTO_THREAD_NAME_MAX)
#   define SIR_MAXPID _NTO_THREAD_NAME_MAX
#  endif
#  if !defined(SIR_MAXPID) && defined(__APPLE__)
#   define SIR_MAXPID 64
#  endif
#  if !defined(SIR_MAXPID) && defined(__OpenBSD__)
#   define SIR_MAXPID 32
#  endif
#  if !defined(SIR_MAXPID) && defined(__SOLARIS__)
#   define SIR_MAXPID 31
#  endif
#  if !defined(SIR_MAXPID) && defined(__linux__)
#   define SIR_MAXPID 16
#  endif
#  if !defined(SIR_MAXPID)
#   define SIR_MAXPID 15
#  endif

#  if (defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0) || \
       defined(__MACOS__) || defined(__OpenBSD__)
#   define SIR_MSEC_TIMER
#   define SIR_MSEC_POSIX
#  else
#   undef SIR_MSEC_TIMER
#  endif

/** The clock used to obtain timestamps. */
#  if defined(CLOCK_REALTIME_FAST)
#   define SIR_WALLCLOCK CLOCK_REALTIME_FAST
#  elif defined(CLOCK_REALTIME_COARSE)
#   define SIR_WALLCLOCK CLOCK_REALTIME_COARSE
#  else
#   define SIR_WALLCLOCK CLOCK_REALTIME
#  endif

/** The clock used to measure intervals. */
#  if defined(CLOCK_UPTIME)
#   define SIR_INTERVALCLOCK CLOCK_UPTIME
#  elif defined(CLOCK_BOOTTIME) && !defined(__EMSCRIPTEN__)
#   define SIR_INTERVALCLOCK CLOCK_BOOTTIME
#  elif defined(CLOCK_HIGHRES)
#   define SIR_INTERVALCLOCK CLOCK_HIGHRES
#  elif defined(CLOCK_MONOTONIC)
#   define SIR_INTERVALCLOCK CLOCK_MONOTONIC
#  else
#   define SIR_INTERVALCLOCK CLOCK_REALTIME
#  endif

/** The plugin handle type. */
typedef void* sir_pluginhandle;

/** The plugin export address type. */
typedef void (*sir_pluginexport)(void);

/** The mutex type. */
typedef pthread_mutex_t sir_mutex;

/** The thread type. */
typedef pthread_t sir_thread;

/** The condition variable type. */
typedef pthread_cond_t sir_condition;

/** The mutex/condition variable wait time type. */
typedef struct timespec sir_wait;

/** The one-time type. */
typedef pthread_once_t sir_once;

/** The one-time execution function type. */
typedef void (*sir_once_fn)(void);

/** The one-time initializer. */
#  define SIR_ONCE_INIT PTHREAD_ONCE_INIT

/** The mutex initializer. */
#  define SIR_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER

# else /* __WIN__ */

#  define SIR_MAXPID 64

#  define SIR_MAXPATH MAX_PATH

#  define SIR_MSEC_TIMER
#  define SIR_MSEC_WIN32
#  define SIR_WALLCLOCK 0
#  define SIR_INTERVALCLOCK 1

/** The plugin handle type. */
typedef HMODULE sir_pluginhandle;

/** The plugin export address type. */
typedef FARPROC sir_pluginexport;

/** The mutex type. */
typedef CRITICAL_SECTION sir_mutex;

/** The thread type. */
typedef HANDLE sir_thread;

/** The condition variable type. */
typedef CONDITION_VARIABLE sir_condition;

/** The mutex/condition variable wait time type. */
typedef DWORD sir_wait;

/** The one-time type. */
typedef INIT_ONCE sir_once;

/** Process/thread ID. */
#  if !defined(__MINGW64__) && !defined(__MINGW32__) && !defined(__ORANGEC__)
typedef int pid_t;
#  endif

/** The one-time execution function type. */
typedef BOOL(CALLBACK* sir_once_fn)(PINIT_ONCE, PVOID, PVOID*);

/** The one-time initializer. */
#  define SIR_ONCE_INIT INIT_ONCE_STATIC_INIT

/** The mutex initializer. */
#  define SIR_MUTEX_INIT {0}

# endif /* !__WIN__ */

# if !defined(_sir_thread_local)
#  if (__STDC_VERSION__ >= 201112 && !defined(__STDC_NO_THREADS__)) || \
     (defined(__SUNPRO_C) || defined(__SUNPRO_CC))
#   if defined(_AIX) && defined(__GNUC__)
#    define _sir_thread_local __thread
#   else
#    define _sir_thread_local _Thread_local
#   endif
#  elif defined(__WIN__)
#   define _sir_thread_local __declspec(thread)
#  elif defined(__GNUC__) || (defined(_AIX) && (defined(__xlC_ver__) || defined(__ibmxl__)))
#   define _sir_thread_local __thread
#  else
#   if !defined(_CH_) && !defined(__CH__)
#    error "unable to resolve thread local attribute; please contact the developers"
#   endif
#  endif
# endif

# if defined(__WIN__) && defined(__STDC_SECURE_LIB__)
#  define __HAVE_STDC_SECURE_OR_EXT1__
# elif defined(__STDC_LIB_EXT1__)
#  define __HAVE_STDC_SECURE_OR_EXT1__
# elif defined(__STDC_ALLOC_LIB__)
#  define __HAVE_STDC_EXT2__
# endif

# if (defined(__clang__) || defined(__GNUC__)) && defined(__FILE_NAME__)
#  define __file__ __FILE_NAME__
# elif defined(__BASE_FILE__)
#  define __file__ __BASE_FILE__
# else
#  define __file__ __FILE__
# endif

#endif /* !_SIR_PLATFORM_H_INCLUDED */

#include "sir/impl.h"

/* Support Clang's -Wdisabled-macro-expansion check on Linux/glibc */
#if defined(SIR_LINT) && (defined(__linux__) && defined(__GLIBC__)) && \
    (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) && \
    (defined(__clang__) || defined(__clang_version__))
# undef stdin
# undef stdout
# undef stderr
#endif

/* End of platform.h */
