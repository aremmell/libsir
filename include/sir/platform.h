/*
 * platform.h
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
# define _SIR_PLATFORM_H_INCLUDED

# if defined(_MSC_VER)
#  include <stddef.h>
#  if defined(_USE_ATTRIBUTES_FOR_SAL)
#   undef _USE_ATTRIBUTES_FOR_SAL
#  endif
#  define _USE_ATTRIBUTES_FOR_SAL 1
#  include <sal.h>
#  define PRINTF_FORMAT _Printf_format_string_
#  define PRINTF_FORMAT_ATTR(fmt_p, va_p) /**/
# else
#  define PRINTF_FORMAT /**/
#  if defined(__MINGW32__) || defined(__MINGW64__)
#   if !defined(__USE_MINGW_ANSI_STDIO)
#    define __USE_MINGW_ANSI_STDIO 1
#   endif
#   define PRINTF_FORMAT_ATTR(fmt_p, va_p) \
    __attribute__((format (gnu_printf, fmt_p, va_p)))
#  else
#   if !defined(__SUNPRO_C) && !defined(__SUNPRO_C)
#    define PRINTF_FORMAT_ATTR(fmt_p, va_p) \
     __attribute__((format (printf, fmt_p, va_p)))
#   else
#    define PRINTF_FORMAT_ATTR(fmt_p, va_p) /**/
#   endif
#  endif
# endif

# if !defined(_WIN32)
#  if defined(__STDC_NO_ATOMICS__)
#   undef __HAVE_ATOMIC_H__
#  else
#   define __HAVE_ATOMIC_H__ /**/
#  endif
#  if defined(__GNUC__)
#   if __GNUC__ <= 4
#    if defined(__GNUC_MINOR__)
#     if __GNUC_MINOR__ <= 8
#      if !defined(__clang_version__)
#       undef __HAVE_ATOMIC_H__
#      endif
#     endif
#    endif
#   endif
#  endif
#  if defined(__DragonFly__)
#   if defined(__clang__) && defined(__clang_version__)
#    undef __HAVE_ATOMIC_H__
#   endif
#  endif
#  if !defined(__open_xl__) && defined(__ibmxl__) && defined(__ibmxl_release__)
#   if __ibmxl__ <= 16
#    if __ibmxl_release__ <= 1
#     undef __HAVE_ATOMIC_H__
#    endif
#   endif
#  endif
#  if !defined(__open_xl__) && defined(__xlC_ver__)
#   if __xlC_ver__ <= 0x0000000e
#    undef __HAVE_ATOMIC_H__
#   endif
#  endif
#  if defined(__STDC_WANT_LIB_EXT1__)
#   undef __STDC_WANT_LIB_EXT1__
#  endif
#  define __STDC_WANT_LIB_EXT1__ 1
#  if defined(__STDC_WANT_LIB_EXT2__)
#   undef __STDC_WANT_LIB_EXT2__
#  endif
#  define __STDC_WANT_LIB_EXT2__ 1
#  if defined(__APPLE__) && defined(__MACH__)
#   define __MACOS__ /**/
#   define _DARWIN_C_SOURCE /**/
#  elif defined(__serenity__)
#   define USE_PTHREAD_GETNAME_NP /**/
#  elif defined(__OpenBSD__)
#   define __BSD__ /**/
#   define __FreeBSD_PTHREAD_NP_11_3__
#  elif defined(__NetBSD__)
#   define __BSD__ /**/
#   if !defined(_NETBSD_SOURCE)
#    define _NETBSD_SOURCE 1
#   endif
#   define USE_PTHREAD_GETNAME_NP /**/
#  elif defined(__FreeBSD__) || defined(__DragonFly__)
#   define __BSD__ /**/
#   define _BSD_SOURCE /**/
#   if !defined(_DEFAULT_SOURCE)
#    define _DEFAULT_SOURCE /**/
#   endif
#   include <sys/param.h>
#   if __FreeBSD_version >= 1202500
#    define __FreeBSD_PTHREAD_NP_12_2__
#   elif __FreeBSD_version >= 1103500
#    define __FreeBSD_PTHREAD_NP_11_3__
#   elif __DragonFly_version >= 400907
#    define __DragonFly_getthreadid__ /**/
#   endif
#   if defined(__DragonFly__)
#    define USE_PTHREAD_GETNAME_NP /**/
#   endif
#  else
#   if defined(__HAIKU__)
#    if !defined(__USE_GNU)
#     define __USE_GNU /**/
#    endif
#    if !defined(_GNU_SOURCE)
#     define _GNU_SOURCE 1
#    endif
#    if defined(__clang__) && !defined(_GNU_PTHREAD_H_)
#     include <pthread.h>
extern /* Workaround a Clang on Haiku bug. */
int pthread_getname_np(pthread_t thread, char* buffer, size_t length);
#    endif
#    define USE_PTHREAD_GETNAME_NP /**/
#   endif
#   if defined(__linux__)
#    if !defined(_GNU_SOURCE)
#     define _GNU_SOURCE 1
#    endif
#    define USE_PTHREAD_GETNAME_NP /**/
#   endif
#   if defined(__CYGWIN__)
#    if !defined(_GNU_SOURCE)
#     define _GNU_SOURCE 1
#    endif
#    define USE_PTHREAD_GETNAME_NP /**/
#    include <sys/features.h>
#   endif
#   if defined(__ANDROID__) && defined(__ANDROID_API__)
#    if __ANDROID_API__ < 26
#     undef USE_PTHREAD_GETNAME_NP
#    endif
#   endif
#   if defined(__illumos__) || ((defined(__sun) || defined(__sun__)) && \
              (defined(__SVR4) || defined(__svr4__)))
#    define __SOLARIS__ /**/
#    define USE_PTHREAD_GETNAME_NP /**/
#    if !defined(_ATFILE_SOURCE)
#     define _ATFILE_SOURCE 1
#    endif
#    if !defined(__EXTENSIONS__)
#     define __EXTENSIONS__ /**/
#    endif
#   endif
#   if !defined(_POSIX_C_SOURCE)
#    define _POSIX_C_SOURCE 200809L
#   endif
#   if !defined(_DEFAULT_SOURCE)
#    define _DEFAULT_SOURCE /**/
#   endif
#   if !defined(_XOPEN_SOURCE)
#    define _XOPEN_SOURCE 700
#   endif
#  endif
# else /* _WIN32 */
#  define __WIN__ /**/
#  define SIR_NO_SYSTEM_LOGGERS /**/
#  undef __HAVE_ATOMIC_H__
#  define __WANT_STDC_SECURE_LIB__ 1
#  define WIN32_LEAN_AND_MEAN
#  define WINVER       0x0A00 /** Windows 10 SDK */
#  define _WIN32_WINNT 0x0A00
#  define _CRT_RAND_S /**/
#  if defined(__MINGW32__) || defined(__MINGW64__)
#   define USE_PTHREAD_GETNAME_NP /**/
#  endif
#  include <windows.h>
#  include <io.h>
#  include <synchapi.h>
#  include <process.h>
#  include <winsock2.h>
#  include <conio.h>
#  include <shlwapi.h>
#  include <direct.h>
#  if defined(__MINGW32__) || defined(__MINGW64__)
#   define __USE_MINGW_ANSI_STDIO 1
#   include <pthread.h>
typedef  /* Workaround a MinGW bug */
void (__cdecl* _invalid_parameter_handler)(
 wchar_t const*, wchar_t const*, wchar_t const*,
 unsigned int, uintptr_t);
_invalid_parameter_handler
_set_thread_local_invalid_parameter_handler(
 _invalid_parameter_handler pNew);
#  endif
# endif

# if !defined(__MACOS__) && !defined(__BSD__) && !defined(__SOLARIS__)
#  define SIR_IMPL_STRLCPY 1
#  define SIR_IMPL_STRLCAT 1
# endif

# if defined(__MINGW64__)
#  define PID_CAST (int)
# else
#  define PID_CAST /**/
# endif

# if defined(_AIX)
#  define CLOCK_CAST (int)
# else
#  define CLOCK_CAST /**/
# endif

# if defined(SIR_ASSERT_ENABLED)
#  include <assert.h>
#  define SIR_ASSERT(...) assert(__VA_ARGS__)
# else
#  if defined(SIR_SELFLOG)
#   define SIR_ASSERT(...) \
     if (!(__VA_ARGS__)) { \
         _sir_selflog(BRED("!!! would be asserting: " #__VA_ARGS__ "")); \
     }
#  else
#   define SIR_ASSERT(...)
#  endif
# endif

# include <errno.h>
# include <stdarg.h>
# include <stdbool.h>
# include <stdint.h>
# include <inttypes.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <limits.h>
# include <time.h>

# if !defined(SIR_NO_SYSTEM_LOGGERS)
#  if defined(__MACOS__) && !defined(__GNUC__)
#   define SIR_OS_LOG_ENABLED /**/
#  else
#   undef SIR_OS_LOG_ENABLED
#   define SIR_SYSLOG_ENABLED /**/
#  endif
# else
#  undef SIR_OS_LOG_ENABLED
#  undef SIR_SYSLOG_ENABLED
# endif

# define SIR_MAXHOST 256

# if !defined(__WIN__)
#  include <dlfcn.h>
#  include <pthread.h>
#  if defined(__illumos__)
#   include <sys/fcntl.h>
#  endif
#  if defined(__linux__) && defined(__USE_GNU) && \
             (defined(__SUNPRO_C) || defined(__SUNPRO_CC))
#   undef __USE_GNU
#   include <fcntl.h>
#   define __USE_GNU /**/
#  else
#   include <fcntl.h>
#  endif
#  include <unistd.h>
#  if !defined(__CYGWIN__) && !defined(__HAIKU__) && \
      !defined(__serenity__) && !defined(_AIX)
#   include <sys/syscall.h>
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
#   include <syslog.h>
#  endif
#  if defined(_AIX)
#   include <sys/procfs.h>
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
#   include <sys/_types/_timespec.h>
#   include <mach/mach.h>
#   include <mach/clock.h>
#   include <mach/mach_time.h>
#   if defined(SIR_OS_LOG_ENABLED)
#    include <os/log.h>
#    include <os/trace.h>
#    include <os/activity.h>
#   endif
#  endif
#  include "sir/impl.h"

#  if defined(PATH_MAX)
#   define SIR_MAXPATH PATH_MAX
#  elif defined(MAXPATHLEN)
#   define SIR_MAXPATH MAXPATHLEN
#  else
#   define SIR_MAXPATH 1024
#  endif

#  if defined(__MACOS__)
#   define SIR_MSEC_TIMER /**/
#   define SIR_MSEC_MACH /**/
#  elif _POSIX_TIMERS > 0
#   define SIR_MSEC_TIMER /**/
#   define SIR_MSEC_POSIX /**/
#  else
#   undef SIR_MSEC_TIMER
#  endif

/** The plugin handle type. */
typedef void* sir_pluginhandle;

/** The plugin export address type. */
typedef void* sir_pluginexport;

/** The mutex type. */
typedef pthread_mutex_t sir_mutex;

/** The one-time type. */
typedef pthread_once_t sir_once;

/** The one-time execution function type. */
typedef void (*sir_once_fn)(void);

/** The one-time initializer. */
#  define SIR_ONCE_INIT PTHREAD_ONCE_INIT

# else /* __WIN__ */

#  define SIR_MAXPATH MAX_PATH

#  define SIR_MSEC_TIMER /**/
#  define SIR_MSEC_WIN32 /**/

/** The plugin handle type. */
typedef HMODULE sir_pluginhandle;

/** The plugin export address type. */
typedef FARPROC sir_pluginexport;

/** The mutex type. */
typedef HANDLE sir_mutex;

/** The one-time type. */
typedef INIT_ONCE sir_once;

/** Process/thread ID. */
#  if !defined(__MINGW64__) && !defined(__MINGW32__)
typedef int pid_t;
#  endif

/** The one-time execution function type. */
typedef BOOL(CALLBACK* sir_once_fn)(PINIT_ONCE, PVOID, PVOID*);

/** The one-time initializer. */
#  define SIR_ONCE_INIT INIT_ONCE_STATIC_INIT

# endif // !__WIN__

# if (__STDC_VERSION__ >= 201112 && !defined(__STDC_NO_THREADS__)) || \
     (defined(__SUNPRO_C) || defined(__SUNPRO_CC))
#  if defined(_AIX) && defined(__GNUC__)
#   define _sir_thread_local __thread
#  else
#   define _sir_thread_local _Thread_local
#  endif
# elif defined(__WIN__)
#  define _sir_thread_local __declspec(thread)
# elif defined(__GNUC__) || (defined(_AIX) && (defined(__xlC_ver__) || defined(__ibmxl__)))
#  define _sir_thread_local __thread
# else
#  error "unable to resolve thread local attribute; please contact the author."
# endif

# if defined(__WIN__) && defined(__STDC_SECURE_LIB__)
#  define __HAVE_STDC_SECURE_OR_EXT1__
# elif defined(__STDC_LIB_EXT1__)
#  define __HAVE_STDC_SECURE_OR_EXT1__
# elif defined(__STDC_ALLOC_LIB__)
#  define __HAVE_STDC_EXT2__
# endif

# if !defined(__MACOS__)
#  if defined(__linux__) && _POSIX_C_SOURCE >= 199309L
#   define SIR_MSECCLOCK CLOCK_REALTIME
#  else
#   define SIR_MSECCLOCK CLOCK_REALTIME
#  endif
# else /* __MACOS__ */
#  define SIR_MSECCLOCK REALTIME_CLOCK
# endif

# if (defined(__clang__) || defined(__GNUC__)) && defined(__FILE_NAME__)
#  define __file__ __FILE_NAME__
# elif defined(__BASE_FILE__)
#  define __file__ __BASE_FILE__
# else
#  define __file__ __FILE__
# endif

#endif /* !_SIR_PLATFORM_H_INCLUDED */
