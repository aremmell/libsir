/*
 * tests.h
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
#ifndef _SIR_TESTS_H_INCLUDED
# define _SIR_TESTS_H_INCLUDED

# include <sir.h>
# include <sirerrors.h>
# include <sirfilecache.h>
# include <sirinternal.h>
# include <sirfilesystem.h>
# include <sirhelpers.h>
# include <sirtextstyle.h>
# include <siransimacros.h>

# if !defined(__WIN__)
#  include <dirent.h>
#  if defined(CLOCK_MONOTONIC_RAW)
#   define SIRTEST_CLOCK CLOCK_MONOTONIC_RAW
#  else
#   define SIRTEST_CLOCK CLOCK_MONOTONIC
#  endif
# endif

# define INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, p_name, init) \
    sirinit var         = {0}; \
    var.d_stdout.opts   = (o_stdout) > 0 ? o_stdout : SIRO_DEFAULT; \
    var.d_stdout.levels = l_stdout; \
    var.d_stderr.opts   = (o_stderr) > 0 ? o_stderr : SIRO_DEFAULT; \
    var.d_stderr.levels = l_stderr; \
    if (_sir_validstrnofail(p_name)) \
        _sir_strncpy(var.name, SIR_MAXNAME, p_name, SIR_MAXNAME); \
    bool var##_init = false; \
    if (init) \
        var##_init = sir_init(&var);

# define INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, true)

# define INIT_SL(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, false)

# define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
    INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, "")

# define MAKE_LOG_NAME(name) "logs/" name

# define TEST_S(n) ((n) > 1 ? ("test" "s") : "test")
# define PRN_STR(str) ((str) ? (str) : RED("NULL"))
# define PRN_PASS(pass) ((pass) ? GREENB("PASS") : REDB("FAIL"))
# define INDENT_ITEM "\t  " SIR_BULLET " "

# define SIR_MAXTESTNAME 32
# define SIR_MAXCLIFLAG  32
# define SIR_MAXUSAGE    256

/**
 * @defgroup tests Tests
 *
 * libsir integrity tests
 *
 * @addtogroup tests
 * @{
 */

/**
 * @test Properly handle multiple threads competing for locked sections.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_threadrace(void);

/**
 * @test Properly handle messages that exceed internal buffer sizes.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_exceedmaxsize(void);

/**
 * @test Properly handle the lack of any output destinations.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failnooutputdest(void);

/**
 * @test Properly handle null/empty input.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failnulls(void);

/**
 * @test Ensure an empty message is properly handled.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failemptymessage(void);

/**
 * @test Properly handle adding and removing log files.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_filecachesanity(void);

/**
 * @test Properly handle invalid log file name.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failinvalidfilename(void);

/**
 * @test Properly handle log file without appropriate permissions.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failfilebadpermission(void);

/**
 * @test Properly refuse to add a duplicate file.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_faildupefile(void);

/**
 * @test Properly refuse to remove a file that isn't added.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failremovebadfile(void);

/**
 * @test Properly roll/archive a file when it hits max size.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_rollandarchivefile(void);

/**
 * @test Properly handle calls without initialization.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failwithoutinit(void);

/**
 * @test Properly handle two initialization calls without corresponding cleanup.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failinittwice(void);

/**
 * @test Properly handle initialization with junk memory.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failinvalidinitdata(void);

/**
 * @test Properly handle initialization, cleanup, re-initialization.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_initcleanupinit(void);

/**
 * @test Properly handle calls after cleanup.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_failaftercleanup(void);

/**
 * @test Properly return valid codes and messages for all possible errors.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_errorsanity(void);

/**
 * @test Properly style stdio output for each level, and handle style overrides.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_textstylesanity(void);

/**
 * @test Properly reject invalid option bitmasks.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_optionssanity(void);

/**
 * @test Properly reject invalid level bitmasks.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_levelssanity(void);

/**
 * @test Performance evaluation.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_perf(void);

/**
 * @test Properly update levels/options at runtime.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_updatesanity(void);

/**
 * @test Properly open, configure, and send messages to syslog().
 * @note Disabled on Windows and macOS.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_syslog(void);

/**
 * @test Properly open, configure, and send messages to os_log().
 * @note macOS only.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_os_log(void);

/**
 * @test Ensure the proper functionality of portable filesystem implementation.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_filesystem(void);

/**
 * @test Ensure that spam messages are squelched, normal messages are not,
 * and proper return values result from calls into libsir.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_squelchspam(void);

/** @} */

/**
 * @ test
 * @returns bool `true` if the test passed, `false` otherwise.
bool sirtest_xxxx(void); */

/*
 * Error macros and functions
 */

bool print_test_error(bool result, bool expected);
# define print_expected_error() print_test_error(true, true)
# define print_result_and_return(pass) print_test_error(pass, false)

void print_os_error(void);
bool filter_error(bool pass, uint16_t err);

# if !defined(__WIN__)
#  define handle_os_error(clib, fmt, ...)               \
     (void)clib;                                        \
     _sir_handleerr(errno);                             \
     fprintf(stderr, "\t" RED(fmt) ":\n", __VA_ARGS__); \
     print_os_error();
# else /* __WIN__ */
#  define handle_os_error(clib, fmt, ...)                                \
     clib ? _sir_handleerr(errno) : _sir_handlewin32err(GetLastError()); \
     fprintf(stderr, "\t" RED(fmt) ":\n", __VA_ARGS__);                  \
     print_os_error();
# endif

/*
 * Utility functions, macros, and types
 */

/** Function signature for a single test. */
typedef bool (*sir_test_fn)(void);

/**
 * Map a test to a human-readable description. This type constructs the list of
 * tests to be executed.
 */
typedef struct {
    const char* name;
    sir_test_fn fn;
    bool run;
    bool pass;
} sir_test;

/** A simple timer type. */
typedef struct {
# if !defined(__WIN__)
    struct timespec ts;
# else /* __WIN__ */
    FILETIME ft;
# endif
} sir_timer;

/** Arguments passed to worker threads. */
typedef struct {
    char log_file[SIR_MAXPATH];
    bool pass;
} thread_args;

/** Returns a random number from the system's best PRNG, modulus upper_bound */
uint32_t getrand(uint32_t upper_bound);

/** Calls getrand() and returns true if even, false if odd. */
static inline
bool getrand_bool(uint32_t upper_bound) {
    return getrand(upper_bound) % 2 == 0;
}

/** prints a message in green if pass is true, or red otherwise. */
# define PRINT_PASS(pass, msg, ...)      \
    if (pass)                            \
        printf(GREEN(msg), __VA_ARGS__); \
    else                                 \
        printf(RED(msg), __VA_ARGS__);

bool rmfile(const char* filename);
bool deletefiles(const char* search, const char* filename, unsigned* data);
bool countfiles(const char* search, const char* filename, unsigned* data);

typedef bool (*fileenumproc)(const char* search, const char* filename, unsigned* data);
bool enumfiles(const char* search, fileenumproc cb, unsigned* data);

bool sirtimerstart(sir_timer* timer);
float sirtimerelapsed(const sir_timer* timer); // msec
long sirtimergetres(void); // nsec

# if defined(SIR_OS_LOG_ENABLED)
void os_log_parent_activity(void* ctx);
void os_log_child_activity(void* ctx);
# endif

static const struct cl_arg {
    const char* flag;
    const char* usage;
    const char* desc;
    } _cl_arg_list[] = {
        {"--perf",       "", "Only run the performance measurement test."},
        {"--only",       ""  ULINE("name") " [, name, ...]", "Only run the test(s) specified."},
        {"--list",       "", "Prints a list of available test names for use with " BOLD("--only") "."},
        {"--leave-logs", "", "Log files are not deleted so that they may be examined."},
        {"--wait",       "", "Waits for a keypress after running test(s) before exiting."},
        {"--help",       "", "Shows this message."}
    };

bool mark_test_to_run(const char* name);

void print_usage_info(void);
void print_test_list(void);

#endif /* !_SIR_TESTS_H_INCLUDED */
