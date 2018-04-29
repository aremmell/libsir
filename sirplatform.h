/*!
 * \file sirplatform.h
 *
 * Platform configuration for the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */
#ifndef _SIR_PLATFORM_H_INCLUDED
#define _SIR_PLATFORM_H_INCLUDED

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

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
#else
#undef SIR_MSEC_TIMER
#endif

typedef pthread_mutex_t sirmutex_t;

#else
#include <Windows.h>
#define SIR_MAXPATH MAX_PATH
#define SIR_NO_SYSLOG
#undef SIR_MSEC_TIMER

typedef DWORD pid_t;
typedef HANDLE sirmutex_t;
#endif

/*! A sensible (?) constraint for the limit of a file's path. Note that this value
 * is only used in the absence of PATH_MAX. */
#ifndef SIR_MAXPATH
#define SIR_MAXPATH 65535
#endif

#endif /* !_SIR_PLATFORM_H_INCLUDED */
