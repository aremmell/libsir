/*!
 * \file sirplatform.h
 * \brief Platform-specific configuration for the SIR library.
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 */
#ifndef _SIR_PLATFORM_H_INCLUDED
#define _SIR_PLATFORM_H_INCLUDED

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#define __STDC_WANT_LIB_EXT1__ 1
#define __STDC_WANT_SECURE_LIB__ 1

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>

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

typedef int sirerror_t;
typedef pthread_mutex_t sirmutex_t;

#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define SIR_MAXPATH MAX_PATH
#define SIR_NO_SYSLOG
#define SIR_MSEC_TIMER
#define SIR_MSEC_WIN32

typedef DWORD pid_t;
typedef DWORD sirerror_t;
typedef HANDLE sirmutex_t;
#endif

/*! A value that represents the success condition. */
#define SIR_NOERROR (sirerror_t)0

/*! A sensible (?) constraint for the limit of a file's path. Note that this value
 * is only used in the absence of PATH_MAX. */
#ifndef SIR_MAXPATH
#define SIR_MAXPATH 65535
#endif

#ifndef __STDC_VERSION__
#define __STDC_VERSION__ 0
#endif

#ifndef __STDC_SECURE_LIB__
#define __STDC_SECURE_LIB__ 0
#endif

#ifndef __STDC_NO_THREADS__
#define __STDC_NO_THREADS__ 1
#endif
#define strdef(d) #d
#define def(d) strdef(d)
#pragma message "std version: " def(__STDC_VERSION__)", secure lib: " def(__STDC_SECURE_LIB__) ", stdc threads: " def(__STDC_NO_THREADS__)
#if __STDC_VERSION__ >= 201112L
#pragma message "C11 support available."
#ifdef __STDC_SECURE_LIB__ 
#pragma message "C11 Annex K support available." 
#else
#pragma message "NO C11 Annex K support."
#endif
#else
#pragma message "NO C11 support available."
#endif
#endif /* !_SIR_PLATFORM_H_INCLUDED */
