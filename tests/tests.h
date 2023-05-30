/**
 * @file tests.h
 * @brief libsir test suite.
 *
 * This file and accompanying source code originated from <https://github.com/aremmell/libsir>.
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
 * @todo Nice to have
 * 1. A plugin system or public interface for registering custom adapters, for things like:
 *   - Posting high-priority messages to a REST API endpoint.
 *   - Sending high-prirority messages via SMS or push notification.
 * 2. Compressing archived logs with zlib or similar.
 * 3. Deleting archived logs older than _n_ days.
 * 4. A project file for Xcode.
 * 5. Something I didn't think of yet...
 * 
 * ---------------------------------------------------------------------------------------------------------
 */
#ifndef _SIR_TESTS_H_INCLUDED
#define _SIR_TESTS_H_INCLUDED

#include <sir.h>
#include <sirerrors.h>
#include <sirfilecache.h>
#include <sirinternal.h>
#include <sirhelpers.h>

#include <fcntl.h>

#if !defined(_WIN32)
#   include <dirent.h>
#endif

#define STRFMT(clr, s) clr s "\x1b[0m"
#define RED(s) STRFMT("\x1b[1;91m", s)
#define GREEN(s) STRFMT("\x1b[1;92m", s)
#define WHITE(s) STRFMT("\x1b[1;97m", s)
#define BLUE(s) STRFMT("\x1b[1;34m", s)
#define CYAN(s) STRFMT("\x1b[1;36m", s)
#define YELLOW(s) STRFMT("\x1b[1;33m", s)

#define INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, init) \
    sirinit var         = {0};       \
    var.d_stdout.opts   = o_stdout;  \
    var.d_stdout.levels = l_stdout;  \
    var.d_stderr.opts   = o_stderr;  \
    var.d_stderr.levels = l_stderr;  \
    if (strlen(name) > 0)            \
        _sir_strncpy(var.processName, SIR_MAXNAME, name, SIR_MAXNAME); \
    bool var##_init = false; \
    if (init) \
        var##_init = sir_init(&var);

#define INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, true)

#define INIT_SL(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, false)    

#define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
    INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, "")

#define TEST_S(n) (n > 1 ? ("test" "s") : "test")

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
 * @test Properly open, configure, and send messages to syslog().
 */
bool sirtest_syslog(void);

/**
 * @test Properly open, configure, and send messages to os_log().
 * 
 * @note macOS only.
 */
bool sirtest_os_log(void);

/**
 * @test Ensure the proper functionality of portable filesystem implementation.
 */
bool sirtest_filesystem(void);

/** @} */

/**
 * @test
 *
bool sirtest_xxxx(void); */

bool print_test_error(bool result, bool expected);
#define print_expected_error() print_test_error(true, true)
#define print_result_and_return(pass) print_test_error(pass, false)
void print_os_error(void);

#define _STR(s) #s
#define STR(s) _STR(s)

#if !defined(_WIN32)
#   define handle_os_error(clib, fmt, ...) (void)clib; _sir_handleerr(errno); \
        fprintf(stderr,"\t" RED(fmt) ":\n", __VA_ARGS__); \
        print_os_error();
#else
#   define handle_os_error(clib, fmt, ...) clib ? _sir_handleerr(errno) : _sir_handlewin32err(GetLastError()); \
        fprintf(stderr, "\t" RED(fmt) ":\n", __VA_ARGS__); \
        print_os_error();
#endif

unsigned int getrand(void);

bool rmfile(const char* filename);
bool deletefiles(const char* search, const char* filename, unsigned* data);
bool countfiles(const char* search, const char* filename, unsigned* data);

typedef bool (*fileenumproc)(const char* search, const char* filename, unsigned* data);
bool enumfiles(const char* search, fileenumproc cb, unsigned* data);

typedef struct {
#if !defined(_WIN32)
    struct timespec ts;
#else
    FILETIME ft;
#endif
} sirtimer_t;

bool startsirtimer(sirtimer_t* timer);
float sirtimerelapsed(const sirtimer_t* timer); // msec

#endif /* !_SIR_TESTS_H_INCLUDED */
