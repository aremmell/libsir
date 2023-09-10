/*
 * tests_shared.h
 *
 * Functions, types, and macros that are used by both the C and C++ test rigs.
 *
 * Author:    Ryan M. Lederman <lederman@gmail.com>
 * Copyright: Copyright (c) 2018-2023
 * Version:   2.2.4
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
#ifndef _SIR_TESTS_SHARED_H_INCLUDED
# define _SIR_TESTS_SHARED_H_INCLUDED

# include "sir.h"
# include "sir/errors.h"
# include "sir/internal.h"
# include "sir/helpers.h"
# include "sir/ansimacros.h"

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
# define PRN_STR(str) ((str) ? (str) : RED("NULL"))

/** Prints 'PASS' in green if `pass` is true, or 'FAIL' in red otherwise. */
# define PRN_PASS(pass) ((pass) ? GREENB("PASS") : REDB("FAIL"))

/** Useful for printing items in a list. */
# define INDENT_ITEM "\t  " SIR_BULLET " "

/** The maximum length of a test's name. */
# define SIR_MAXTESTNAME 32

/** The maximum length of a command line argument's flag. */
# define SIR_MAXCLIFLAG  32

/** The maximum length of a command line argument's usage message. */
# define SIR_MAXCLIUSAGE 256

/** Prints `msg` indented with a tab. Used for output during a test's execution. */
# define TEST_MSG(msg, ...) (void)printf("\t" msg "\n", __VA_ARGS__)

/** TEST_MSG but no varargs. Use when `msg` is just a string. */
# define TEST_MSG_0(msg) (void)printf("\t" msg "\n")

/** Prints `msg` in red to stderr. */
# define ERROR_MSG(msg, ...) (void)fprintf(stderr, RED(msg) "\n", __VA_ARGS__)

/** ERROR_MSG but no varargs. Use when `msg` is just a string. */
# define ERROR_MSG_0(msg) (void)fprintf(stderr, RED(msg) "\n")

/** Prints `msg` in green if `expr` is true, or in red if false. */
# define PASSFAIL_MSG(expr, msg, ...) \
    (void)printf(expr ? GREEN(msg) : RED(msg), __VA_ARGS__)

/**
 * Functions
 */

/** Prints an intro message before any test(s) have been executed. */
static inline
void print_intro(size_t tgt_tests) {
    (void)printf(WHITEB("\n" ULINE("libsir") " %s (%s) running %zu %s...") "\n",
        sir_getversionstring(), (sir_isprerelease() ? "prerelease" : "release"),
        tgt_tests, TEST_S(tgt_tests));
}

/** Prints a progress message preceding a test's execution. */
static inline
void print_test_intro(size_t num, size_t tgt_tests, const char* const name) {
    (void)printf(WHITEB("\n(%zu/%zu) '%s'...") "\n\n", num, tgt_tests, name);
}

/** Prints a progress message following a test's execution. */
static inline
void print_test_outro(size_t num, size_t tgt_tests, const char* const name, bool pass) {
    (void)printf(WHITEB("\n(%zu/%zu) '%s' finished: ") "%s\n", num, tgt_tests, name,
                PRN_PASS(pass));
}

/** Prints a message after all test(s) have been executed. */
static inline
void print_test_summary(size_t tgt_tests, size_t passed, double elapsed) {
    elapsed = (elapsed / 1e3);
    if (tgt_tests == passed) {
        (void)printf("\n" WHITEB("done: ")
            GREENB("%s%zu " ULINE("libsir") " %s " EMPH("passed") " in %.03fsec!")
            "\n\n", tgt_tests > 1 ? "all " : "", tgt_tests, TEST_S(tgt_tests), elapsed);
    } else {
        (void)printf("\n" WHITEB("done: ")
            REDB("%zu of %zu " ULINE("libsir") " %s " EMPH("failed") " in %.03fsec")
            "\n\n", tgt_tests - passed, tgt_tests, TEST_S(tgt_tests), elapsed);
    }
}

/** Prints a message if any tests failed, announcing the failed test(s). */
static inline
void print_failed_test_intro(size_t tgt_tests, size_t passed) {
    (void)printf(REDB("Failed %s:") "\n\n", TEST_S(tgt_tests - passed));
}

/** Printes a message containing the name of a failed test. */
static inline
void print_failed_test(const char* const name) {
    (void)printf(RED(INDENT_ITEM "%s\n"), name);
}

/**
 * Types
 */

# if !defined(__cplusplus)
typedef bool (*sir_test_fn)(void); /** Function signature for a single test. */
# else
typedef bool (*sir_test_fn)(); /** Function signature for a single test. */
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

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_TESTS_SHARED_H_INCLUDED */
