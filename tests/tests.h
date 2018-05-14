/**
 * @file tests.h
 * @brief libsir test suite.
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
 * 
 * ---------------------------------------------------------------------------------------------------------
 * 
 * @todo Hardening and Compatibility
 * I have only compiled and tested libsir on the following:
 * - Ubuntu 16.04 x64 (gcc)
 * - Windows 10 x64  (MinGW)
 * - macOS 10.13.4 x64 (clang)
 * #### Other platforms, such as `BSD` and `macOS` remain untested, and probably won't even compile cleanly.
 *
 * @todo Nice to have
 * 1. A plugin system or public interface for registering custom adapters, for things like:
 *   - Posting high-priority messages to a REST API endpoint.
 *   - Sending high-prirority messages via SMS or push notification.
 * 2. Compressing archived logs with zlib or similar.
 * 3. Deleting archived logs older than _n_ days.
 * 4. A project file for Visual Studio.
 * 5. A project file for Xcode.
 * 6. An accompanying C++ wrapper.
 * 7. Something I didn't think of yet.
 * 
 * ---------------------------------------------------------------------------------------------------------
 */
#ifndef _SIR_TESTS_H_INCLUDED
#define _SIR_TESTS_H_INCLUDED

#ifndef _POSIX_C_SOURCE
#   define _POSIX_C_SOURCE 200809L
#endif
#ifndef _DEFAULT_SOURCE
#   define _DEFAULT_SOURCE 1
#endif

#define _CRT_RAND_S

#include "tests.h"
#include "../sir.h"
#include "../sirerrors.h"
#include "../sirfilecache.h"
#include "../sirinternal.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#else
#   define _WIN32_WINNT 0x0600
#include <process.h>
#include <windows.h>
#endif

#ifndef _WIN32
#   define STRFMT(clr, s) clr s "\033[0m"
#   define RED(s) STRFMT("\033[1;91m", s)
#   define GREEN(s) STRFMT("\033[1;92m", s)
#   define WHITE(s) STRFMT("\033[1;97m", s)
#   define BLUE(s) STRFMT("\033[1;34m", s)
#else
#   define RED(s) s
#   define GREEN(s) s
#   define WHITE(s) s
#   define BLUE(s) s
#endif

#define INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    sirinit var         = {0};                          \
    var.d_stdout.opts   = o_stdout;                     \
    var.d_stdout.levels = l_stdout;                     \
    var.d_stderr.opts   = o_stderr;                     \
    var.d_stderr.levels = l_stderr;                     \
    if (strlen(name) > 0)                               \
        strncpy(var.processName, name, SIR_MAXNAME);    \
    bool var##_init     = sir_init(&var);

#define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
    INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, "")

/** Function signature for a single test. */
typedef bool (*sir_test_fn)(void);

/** Map a test to a human-readable description. */
typedef struct {
    const char* name;
    sir_test_fn fn;
} sir_test;

/**
 * @defgroup tests Tests
 * 
 * libsir integrity tests.
 * 
 * @addtogroup tests
 * @{
 */

/**
 * @test Properly handle multiple threads competing for locked sections.
 */
bool sirtest_mthread_race(void);

/**
 * @test Properly handle messages that exceed internal buffer sizes.
 */
bool sirtest_exceedmaxsize(void);

/**
 * @test Properly handle adding and removing log files.
 */
bool sirtest_filecachesanity(void);

/**
 * @test Properly handle invalid text style.
 */
bool sirtest_failsetinvalidstyle(void);

/**
 * @test Properly handle the lack of any output destinations.
 */
bool sirtest_failnooutputdest(void);

/**
 * @test Properly handle invalid log file name.
 */
bool sirtest_failinvalidfilename(void);

/**
 * @test Properly handle log file without appropriate permissions.
 */
bool sirtest_failfilebadpermission(void);

/**
 * @test Properly handle null/empty input.
 */
bool sirtest_failnulls(void);

/**
 * @test Properly handle calls without initialization.
 */
bool sirtest_failwithoutinit(void);

/**
 * @test Properly handle two initialization calls without corresponding cleanup.
 */
bool sirtest_failinittwice(void);

/**
 * @test Properly handle calls after cleanup.
 */
bool sirtest_failaftercleanup(void);

/**
 * @test Properly handle initialization, cleanup, re-initialization.
 */
bool sirtest_initcleanupinit(void);

/**
 * @test Properly refuse to add a duplicate file.
 */
bool sirtest_faildupefile(void);

/**
 * @test Properly refuse to remove a file that isn't added.
 */
bool sirtest_failremovebadfile(void);

/**
 * @test Properly roll/archive a file when it hits max size.
 */
bool sirtest_rollandarchivefile(void);

/**
 * @test Properly return valid codes and messages for all possible errors.
 */
bool sirtest_errorsanity(void);

/**
 * @test Properly style stdio output for each level, and handle style overrides.
 */
bool sirtest_textstylesanity(void);

/**
 * @test Performance evaluation.
 */
bool sirtest_perf(void);

/**
 * @test Properly update levels/options at runtime.
 */
bool sirtest_updatesanity(void);

/**
 * @test .
 *
bool sirtest_xxxx(void); */

/** @} */

bool printerror(bool pass);
void printexpectederr(void);

int getoserr(void);
unsigned int getrand(void);

bool rmfile(const char* filename);
bool deletefiles(const char* search, const char* filename, unsigned* data);
bool countfiles(const char* search, const char* filename, unsigned* data);

typedef bool (*fileenumproc)(const char* search, const char* filename, unsigned* data);
bool enumfiles(const char* search, fileenumproc cb, unsigned* data);

typedef struct {
#ifndef _WIN32
    struct timespec ts;
#else
    FILETIME ft;
#endif
} sirtimer_t;

bool startsirtimer(sirtimer_t* timer);
float sirtimerelapsed(const sirtimer_t* timer); // msec

#endif /* !_SIR_TESTS_H_INCLUDED */
