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
# define SIR_CL_MAXFLAG  32

/** The maximum length of a command line argument's usage message. */
# define SIR_CL_MAXUSAGE 256

# define SIR_CL_PERFFLAG      "--perf"
# define SIR_CL_ONLYFLAG      "--only"
# define SIR_CL_LISTFLAG      "--list"
# define SIR_CL_LEAVELOGSFLAG "--leave-logs"
# define SIR_CL_WAITFLAG      "--wait"
# define SIR_CL_VERSIONFLAG   "--version"
# define SIR_CL_HELPFLAG      "--help"

# define SIR_CL_ONLYUSAGE ULINE("name") " [, " ULINE("name") ", ...]"

# define SIR_CL_PERFDESC      "Only run the performance measurement test"
# define SIR_CL_ONLYDESC      "Only run the test(s) specified"
# define SIR_CL_LISTDESC      "Prints a list of available test names for use with '" BOLD("--only") "'"
# define SIR_CL_LEAVELOGSDESC "Log files are not deleted so that they may be examined"
# define SIR_CL_WAITDESC      "After running test(s), wait for a keypress before exiting"
# define SIR_CL_VERSIONDESC   "Prints the version of libsir that the test suite was built with"
# define SIR_CL_HELPDESC      "Shows this message"

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
 * Marks a test to be executed during this test run. Returns false if unable
 * to locate the requested test.
 */
static inline
bool mark_test_to_run(const char* const name, sir_test* tests, size_t num_tests) {
    bool found = false;
    for (size_t t = 0; t < num_tests; t++) {
        if (_sir_strsame(name, tests[t].name,
            strnlen(tests[t].name, SIR_MAXTESTNAME))) {
            found = tests[t].run = true;
            break;
        }
    }
    return found;
}

/** Prints a list of tests. */
static inline
void print_test_list(sir_test* tests, size_t num_tests) {
    size_t longest = 0;
    for (size_t i = 0; i < num_tests; i++) {
        size_t len = strnlen(tests[i].name, SIR_MAXTESTNAME);
        if (len > longest)
            longest = len;
    }

    (void)printf("\n" WHITE("Available tests:") "\n\n");

    for (size_t i = 0; i < num_tests; i++) {
        (void)printf("\t%s\t", tests[i].name);

        size_t len = strnlen(tests[i].name, SIR_MAXTESTNAME);
        if (len < longest)
            for (size_t n = len; n < longest; n++)
                (void)printf(" ");

        if ((i % 2) != 0 || i == num_tests - 1)
            (void)printf("\n");
    }

    (void)printf("\n");
}

/** Prints usage information. */
static inline
void print_usage_info(const sir_cl_arg* args, size_t num_args) {
    size_t longest = 0;
    for (size_t i = 0; i < num_args; i++) {
        size_t len = strnlen(args[i].flag, SIR_CL_MAXFLAG);
        if (len > longest)
            longest = len;
    }

    (void)fprintf(stderr, "\n" WHITE("Usage:") "\n\n");

    for (size_t i = 0; i < num_args; i++) {
        (void)fprintf(stderr, "\t%s ", args[i].flag);

        size_t len = strnlen(args[i].flag, SIR_CL_MAXFLAG);
        if (len < longest)
            for (size_t n = len; n < longest; n++)
                (void)fprintf(stderr, " ");

        (void)fprintf(stderr, "%s%s%s\n", args[i].usage,
            strnlen(args[i].usage, SIR_CL_MAXUSAGE) > 0 ? " " : "", args[i].desc);
    }

    (void)fprintf(stderr, "\n");
}

/**
 * Prints the last error that occurred on the current thread (if `result` is false
 * and an error was found). Prints the error in green if `expected` is true, or
 * in red if false. Returns `result`.
 */
static inline
bool print_test_error(bool result, bool expected) {
    char msg[SIR_MAXERROR] = {0};
    uint16_t code          = sir_geterror(msg);
    if (SIR_E_NOERROR != code) {
        if (!expected && !result)
            TEST_MSG(RED("!! Unexpected (%"PRIu16", %s)"), code, msg);
        else if (expected)
            TEST_MSG(GREEN("Expected (%"PRIu16", %s)"), code, msg);
    }
    return result;
}

/**
 * Prints the libsir version.
 */
static inline
void print_libsir_version(void) {
    (void)printf("\n"ULINE("libsir") " %s (%s)\n\n", sir_getversionstring(),
        sir_isprerelease() ? "prerelease" : "release");
}

/**
 * Looks up a command line argument by flag. Returns NULL if no matching argument
 * was found.
 */
static inline
const sir_cl_arg* find_cl_arg(const char* flag, const sir_cl_arg* args, size_t num_args) {
    for (size_t n = 0; n < num_args; n++) {
        const sir_cl_arg* arg = &args[n];
        if (_sir_strsame(flag, arg->flag, strnlen(arg->flag, SIR_CL_MAXFLAG)))
            return arg;
    }
    return NULL;
}

/**
 * Parses the command line arguments and calls the appropriate function(s) and/or
 * returns a configuration to be used by the test rig. Returns true if the test rig
 * should proceed, or false if it should exit with EXIT_FAILURE.
 */
static inline
bool parse_cmd_line(int argc, char** argv, const sir_cl_arg* args, size_t num_args,
    sir_test* tests, size_t num_tests, sir_cl_config* config) {

    if (!argv || !args || !tests || !config)
        return false;

    memset(config, 0, sizeof(sir_cl_config));

    for (int n = 1; n < argc; n++) {
        const sir_cl_arg* this_arg = find_cl_arg(argv[n], args, num_args);
        if (!this_arg) {
            ERROR_MSG("unknown option '%s'", argv[n]);
            print_usage_info(args, num_args);
            return false;
        }
        if (_sir_strsame(this_arg->flag, SIR_CL_PERFFLAG, strlen(SIR_CL_PERFFLAG))) {
            config->only = mark_test_to_run("performance", tests, num_tests);
            if (config->only)
                config->to_run = 1;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_ONLYFLAG, strlen(SIR_CL_ONLYFLAG))) {
            while (++n < argc) {
                if (!_sir_validstrnofail(argv[n]))
                    continue;
                if (*argv[n] == '-' || !mark_test_to_run(argv[n], tests, num_tests)) {
                    ERROR_MSG("invalid argument to %s: '%s'", SIR_CL_ONLYFLAG, argv[n]);
                    print_usage_info(args, num_args);
                    return false;
                }
                config->to_run++;
            }
            if (0 == config->to_run) {
                ERROR_MSG("value expected for '%s'", SIR_CL_ONLYFLAG);
                print_usage_info(args, num_args);
                return false;
            }
        } else if (_sir_strsame(this_arg->flag, SIR_CL_LISTFLAG, strlen(SIR_CL_LISTFLAG))) {
            print_test_list(tests, num_tests);
            return false;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_LEAVELOGSFLAG, strlen(SIR_CL_LEAVELOGSFLAG))) {
            config->leave_logs = true;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_WAITFLAG, strlen(SIR_CL_WAITFLAG))) {
            config->wait = true;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_VERSIONFLAG, strlen(SIR_CL_VERSIONFLAG))) {
            print_libsir_version();
            return false;
        } else if (_sir_strsame(this_arg->flag, SIR_CL_HELPFLAG, strlen(SIR_CL_HELPFLAG))) {
            print_usage_info(args, num_args);
            return false;
        } else {
            ERROR_MSG("unknown option '%s'", this_arg->flag);
            return false;
        }
    }
    return true;
}

/** Waits for a keypress. */
static inline
void wait_for_keypress(void) {
    (void)printf(WHITEB(EMPH("press any key to exit...")) "\n");
    char ch = '\0';
    (void)_sir_getchar(&ch);
    SIR_UNUSED(ch);
}

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
static inline
long sir_timer_getres(void) {
    long retval = 0L;
#if !defined(__WIN__)
    struct timespec res;
    int get = clock_getres(SIR_INTERVALCLOCK, &res);
    SIR_ASSERT(0 == get);
    if (0 == get)
        retval = res.tv_nsec;
#else /* __WIN__ */
    LARGE_INTEGER cntr_freq;
    (void)QueryPerformanceFrequency(&cntr_freq);
    if (cntr_freq.QuadPart <= 0)
        retval = 0L;
    else
        retval = (long)(ceil(((double)cntr_freq.QuadPart) / 1e9));
#endif
    return retval;
}

/** Puts the current thread to sleep for `msec` milliseconds. */
static inline
void sir_sleep_msec(uint32_t msec) {
    if (0U == msec)
        return;

#if !defined(__WIN__)
    struct timespec ts = { msec / 1000, (msec % 1000) * 1000000 };
    (void)nanosleep(&ts, NULL);
#else /* __WIN__ */
    (void)SleepEx((DWORD)msec, TRUE);
#endif
}

/** Reads a line from the open stream `f` and stores it in `buf`. */
static inline
size_t sir_readline(FILE* f, char* buf, size_t size) {
    if (!f || !buf || 0 == size)
        return 0;

    int ch     = 0;
    size_t idx = 0;

    while (idx < size) {
        ch = getc(f);
        if (EOF == ch || '\n' == ch)
            break;
        buf[idx++] = (char)ch;
    }

    return (0 == ferror(f)) ? idx : 0;
}

/** Returns a random number from the system's best PRNG, modulus upper_bound */
static inline
uint32_t getrand(uint32_t upper_bound) {
#if !defined(__WIN__) || defined(__EMBARCADEROC__)
# if defined(__MACOS__) || defined(__BSD__) || defined(__serenity__) || \
     defined(__SOLARIS__) || defined(__ANDROID__) || defined(__CYGWIN__) || \
     (defined(__linux__) && defined(__GLIBC__) && GLIBC_VERSION >= 23600)
    if (upper_bound < 2U)
        upper_bound = 2U;
    return arc4random_uniform(upper_bound);
# else
#  if defined(__EMBARCADEROC__)
    return (uint32_t)(random(upper_bound));
#  else
    return (uint32_t)(random() % upper_bound);
#  endif
# endif
#else /* __WIN__ */
    uint32_t ctx = 0;
    if (0 != rand_s(&ctx))
        ctx = (uint32_t)rand();
    return ctx % upper_bound;
#endif
}

/** Calls getrand() and returns true if even, false if odd. */
static inline
bool getrand_bool(uint32_t upper_bound) {
    return ((!upper_bound) ? true : getrand(upper_bound) % 2 == 0);
}

# if defined(__cplusplus)
}
# endif

#endif /* !_SIR_TESTS_SHARED_H_INCLUDED */
