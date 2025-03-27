/*
 * tests_shared.h
 *
 * Functions, types, and macros that are used by both the C and C++ test rigs.
 *
 * Version: 2.2.6
 *
 * ----------------------------------------------------------------------------
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
 * ----------------------------------------------------------------------------
 */

#ifndef _SIR_TESTS_SHARED_H_INCLUDED
# define _SIR_TESTS_SHARED_H_INCLUDED

# include "sir.h"
# include "sir/errors.h"
# include "sir/internal.h"
# include "sir/helpers.h"
# include "sir/ansimacros.h"
# include "sir/filesystem.h"

# if !defined(__WIN__) || defined(__ORANGEC__)
#  if !defined(__ORANGEC__)
#   include <dirent.h>
#  endif
# elif defined(__WIN__)
#  include <math.h>
# endif

# if defined(__cplusplus)
extern "C" {
# endif

/**
 * Macros
 */

/** The directory in which log files are saved (relative to repo root). */
# define SIR_TESTLOGDIR "./logs/"

/** Creates a relative path for the specified log file name. */
# define MAKE_LOG_NAME(name) SIR_TESTLOGDIR name

/** Returns 'test' or 'tests' depending on whether n is plural. */
# define TEST_S(n) ((n) > 1 ? "tests" : "test")

/** Helper for printing a possibly-null string. */
# define PRN_STR(str) ((str) ? (str) : SIR_RED("NULL"))

/** Prints 'PASS' in green if `pass` is true, or 'FAIL' in red otherwise. */
# define PRN_PASS(pass) ((pass) ? SIR_GREENB("PASS") : SIR_REDB("FAIL"))

/** Useful for printing items in a list. */
# define INDENT_ITEM "\t  " SIR_BULLET " "

/** The maximum length of a test's name. */
# define SIR_MAXTESTNAME 32

/** The maximum length of a command line argument's flag. */
# define SIR_CL_MAXFLAG  32

/** The maximum length of a command line argument's usage message. */
# define SIR_CL_MAXUSAGE 256

/**
 * Command line argument flags.
 */

# define SIR_CL_PERFFLAG      "--perf"
# define SIR_CL_ONLYFLAG      "--only"
# define SIR_CL_LISTFLAG      "--list"
# define SIR_CL_LEAVELOGSFLAG "--leave-logs"
# define SIR_CL_WAITFLAG      "--wait"
# define SIR_CL_VERSIONFLAG   "--version"
# define SIR_CL_HELPFLAG      "--help"

/**
 * Command line usage messages.
 */

# define SIR_CL_ONLYUSAGE     SIR_ULINE("name") " [, " SIR_ULINE("name") ", ...]"

/** The name for the performance test. It is referenced by name in the
 * CLI parser, so this is our single source of truth. */
# define SIR_CL_PERFNAME      "performance"

/**
 * Command line descriptions.
 */

# define SIR_CL_PERFDESC      "Only run the performance measurement test"
# define SIR_CL_ONLYDESC      "Only run the test(s) specified"
# define SIR_CL_LISTDESC      "Prints a list of available test names for use with '" SIR_BOLD("--only") "'"
# define SIR_CL_LEAVELOGSDESC "Log files are not deleted so that they may be examined"
# define SIR_CL_WAITDESC      "After running test(s), wait for a keypress before exiting"
# define SIR_CL_VERSIONDESC   "Prints the version of libsir that the test suite was built with"
# define SIR_CL_HELPDESC      "Shows this message"

/** Prints `msg` indented with a tab. Used for output during a test's execution. */
# define TEST_MSG(msg, ...) (void)printf("\t" msg SIR_EOL, __VA_ARGS__)

/** TEST_MSG but no varargs. Use when `msg` is just a string. */
# define TEST_MSG_0(msg) (void)printf("\t" msg SIR_EOL)

/** Prints `msg` in red to stderr. */
# define ERROR_MSG(msg, ...) TEST_MSG(SIR_RED(msg), __VA_ARGS__)

/** ERROR_MSG but no varargs. Use when `msg` is just a string. */
# define ERROR_MSG_0(msg) TEST_MSG_0(SIR_RED(msg))

/** Prints `msg` in green if `expr` is true, or in red if false. */
# if !defined(SIR_NO_TEXT_STYLING)
#  define PASSFAIL_MSG(expr, msg, ...) \
     (void)printf(expr ? SIR_GREEN(msg) : SIR_RED(msg), __VA_ARGS__)
# else
#  define PASSFAIL_MSG(expr, msg, ...) \
     (void)printf(msg, __VA_ARGS__)
# endif

/** Prints an expected error in green. */
# define PRINT_EXPECTED_ERROR() (void)print_test_error(true, true)

/**
 * If any error has occurred, it is printed (see print_test_error). Returns
 * `pass`. Normally used in the return statement at the end of a test:
 *   `return PRINT_RESULT_RETURN(pass);`
 */
# define PRINT_RESULT_RETURN(pass) print_test_error(pass, false)

/**
 * Causes libsir to trap the last OS error that occurred, then prints the error
 * along with a message containing contextual information.
 */
# if !defined(__WIN__)
#  define HANDLE_OS_ERROR(clib, msg, ...) \
    do { \
        SIR_UNUSED(clib); \
        (void)_sir_handleerr(errno); \
        ERROR_MSG(msg ":", __VA_ARGS__); \
        (void)print_test_error(false, false); \
    } while (false)
# else /* __WIN__ */
#  define HANDLE_OS_ERROR(clib, msg, ...) \
    do { \
        clib ? (void)_sir_handleerr(errno) : (void)_sir_handlewin32err(GetLastError()); \
        ERROR_MSG(msg ":", __VA_ARGS__); \
        (void)print_test_error(false, false); \
    } while (false)
# endif

/**
 * Types
 */

 /** Function signature for a single test. */
# if !defined(__cplusplus)
typedef bool (*sir_test_fn)(void);
# else
typedef bool (*sir_test_fn)();
# endif

/**
 * Maps a test function to a human-readable name and some metadata.
 * Comprises the list of tests to be executed.
 */
typedef struct {
    const char* const name; /** The test's name. */
    sir_test_fn fn;         /** The function pointer. */
    bool run;               /** Whether to run the test or not. */
    bool pass;              /** Whether the test passed or not. */
} sir_test;

/** A command line argument. */
typedef struct {
    const char* const flag;  /** (e.g. '--help'). */
    const char* const usage; /** (e.g. [foo, ...]). */
    const char* const desc;  /** (e.g. displays a purple dinosaur). */
} sir_cl_arg;

/** Shared command line configuration. */
typedef struct {
    bool leave_logs; /** --leave-logs */
    bool wait;       /** --wait */
    bool only;       /** --only */
    size_t to_run;   /** Number of test(s) to run. */
} sir_cl_config;

/**
 * Functions
 */

/** Prints an intro message before any test(s) have been executed. */
void print_intro(size_t tgt_tests);

/** Prints a progress message preceding a test's execution. */
void print_test_intro(size_t num, size_t tgt_tests, const char* const name);

/** Prints a progress message following a test's execution. */
void print_test_outro(size_t num, size_t tgt_tests, const char* const name, bool pass);

/** Prints a message after all test(s) have been executed. */
void print_test_summary(size_t tgt_tests, size_t passed, double elapsed);

/** Prints a message if any tests failed, announcing the failed test(s). */
void print_failed_test_intro(size_t tgt_tests, size_t passed);

/** Prints a message containing the name of a failed test. */
void print_failed_test(const char* const name);

/**
 * Marks a test to be executed during this test run. Returns false if unable
 * to locate the requested test.
 */
bool mark_test_to_run(const char* const name, sir_test* tests, size_t num_tests);

/** Prints a list of tests. */
void print_test_list(const sir_test* tests, size_t num_tests);

/** Prints usage information. */
void print_usage_info(const sir_cl_arg* args, size_t num_args);

/**
 * Prints the last error that occurred on the current thread (if `result` is false
 * and an error was found). Prints the error in green if `expected` is true, or
 * in red if false. Returns `result`.
 */
bool print_test_error(bool result, bool expected);

/**
 * Prints the libsir version.
 */
void print_libsir_version(void);

/**
 * Looks up a command line argument by flag. Returns NULL if no matching argument
 * was found.
 */
const sir_cl_arg* find_cl_arg(const char* flag, const sir_cl_arg* args, size_t num_args);

/**
 * Parses the command line arguments and calls the appropriate function(s) and/or
 * returns a configuration to be used by the test rig. Returns true if the test rig
 * should proceed, or false if it should exit with EXIT_FAILURE.
 */
bool parse_cmd_line(int argc, char** argv, const sir_cl_arg* args, size_t num_args,
    sir_test* tests, size_t num_tests, sir_cl_config* config);

/** Waits for a keypress. */
void wait_for_keypress(void);

/** Stores the current time in `timer`. */
static inline
void sir_timer_start(sir_time* timer) {
    (void)_sir_msec_since(NULL, timer);
}

/** Returns the time elapsed since `timer` in milliseconds. */
static inline
double sir_timer_elapsed(const sir_time* timer) {
    sir_time now;
    return _sir_msec_since(timer, &now);
}

/** Returns the resolution of the system timer in nanoseconds. */
long sir_timer_getres(void);

/** Puts the current thread to sleep for `msec` milliseconds. */
void sir_sleep_msec(uint32_t msec);

/** Reads a line from the open stream `f` and stores it in `buf`. */
size_t sir_readline(FILE* f, char* buf, size_t size);

/** Returns a random number from the system's best PRNG, modulus upper_bound */
uint32_t getrand(uint32_t upper_bound);

/** Calls getrand() and returns true if even, false if odd. */
static inline
bool getrand_bool(uint32_t upper_bound) {
    return ((!upper_bound) ? true : getrand(upper_bound) % 2 == 0);
}

/**
 * Deletes the file `filename`, unless `leave_logs` is true, in which case
 * a message is printed and the file is left untouched.
 */
void rmfile(const char* filename, bool leave_logs);

/**
 * Enumerates files at `path`, and if the filename contains `search`, `count` is
 * incremented. If `del` is true, the file is deleted.
 */
bool enumfiles(const char* path, const char* search, bool del, unsigned* count);

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_TESTS_SHARED_H_INCLUDED */
