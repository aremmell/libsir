/*
 * tests.h
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

#ifndef _SIR_TESTS_H_INCLUDED
# define _SIR_TESTS_H_INCLUDED

# include "tests_shared.h"
# include "sir/filecache.h"
# include "sir/textstyle.h"
# include "sir/mutex.h"
# include "sir/threadpool.h"
# include "sir/queue.h"

/**
 * @defgroup tests Tests
 *
 * libsir integrity tests
 *
 * @addtogroup tests
 * @{
 */

/**
 * @test sirtest_threadrace
 * @brief Properly handle multiple threads competing for locked sections.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_threadrace(void);

/**
 * @test sirtest_exceedmaxsize
 * @brief Properly handle messages that exceed internal buffer sizes.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_exceedmaxsize(void);

/**
 * @test sirtest_logwritesanity
 * @brief Ensure the correct (and complete) message is written to a log file.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_logwritesanity(void);

/**
 * @test sirtest_failnooutputdest
 * @brief Properly handle the lack of any output destinations.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failnooutputdest(void);

/**
 * @test sirtest_failnulls
 * @brief Properly handle null/empty input.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failnulls(void);

/**
 * @test sirtest_failemptymessage
 * @brief Ensure an empty message is properly handled.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failemptymessage(void);

/**
 * @test sirtest_filecachesanity
 * @brief Properly handle adding and removing log files.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_filecachesanity(void);

/**
 * @test sirtest_failinvalidfilename
 * @brief Properly handle invalid log file name.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failinvalidfilename(void);

/**
 * @test sirtest_failfilebadpermission
 * @brief Properly handle log file without appropriate permissions.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failfilebadpermission(void);

/**
 * @test sirtest_faildupefile
 * @brief Properly refuse to add a duplicate file.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_faildupefile(void);

/**
 * @test sirtest_failremovebadfile
 * @brief Properly refuse to remove a file that isn't added.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failremovebadfile(void);

/**
 * @test sirtest_rollandarchivefile
 * @brief Properly roll/archive a file when it hits max size.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_rollandarchivefile(void);

/**
 * @test sirtest_failwithoutinit
 * @brief Properly handle calls without initialization.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failwithoutinit(void);

/**
 * @test Correctly determines the initialized state.
 * @returns bool `true` if the test passed, `false` otherwise.
 */
bool sirtest_isinitialized(void);

/**
 * @test sirtest_failinittwice
 * @brief Properly handle two initialization calls without corresponding cleanup.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failinittwice(void);

/**
 * @test sirtest_failinvalidinitdata
 * @brief Properly handle initialization with junk memory.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failinvalidinitdata(void);

/**
 * @test sirtest_initcleanupinit
 * @brief Properly handle initialization, cleanup, re-initialization.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_initcleanupinit(void);

/**
 * @test sirtest_initmakeinit
 * @brief Properly initialize using sir_makeinit.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_initmakeinit(void);

/**
 * @test sirtest_failaftercleanup
 * @brief Properly handle calls after cleanup.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_failaftercleanup(void);

/**
 * @test sirtest_errorsanity
 * @brief Properly return valid codes and messages for all possible errors.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_errorsanity(void);

/**
 * @test sirtest_textstylesanity
 * @brief Properly style stdio output for each level, and handle style overrides.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_textstylesanity(void);

/**
 * @test sirtest_optionssanity
 * @brief Properly reject invalid option bitmasks.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_optionssanity(void);

/**
 * @test sirtest_levelssanity
 * @brief Properly reject invalid level bitmasks.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_levelssanity(void);

/**
 * @test sirtest_mutexsanity
 * @brief Ensure that the mutex implementation is functioning properly.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_mutexsanity(void);

/**
 * @test sirtest_perf
 * @brief Performance evaluation.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_perf(void);

/**
 * @test sirtest_updatesanity
 * @brief Properly update levels/options at runtime.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_updatesanity(void);

/**
 * @test sirtest_threadidsanity
 * @brief Properly format thread ID/names in output.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_threadidsanity(void);

/**
 * @test sirtest_syslog
 * @brief Properly open, configure, and send messages to syslog().
 * @note Disabled on Windows (and macOS if not compiled with Clang.).
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_syslog(void);

/**
 * @test sirtest_os_log
 * @brief Properly open, configure, and send messages to os_log().
 * @note Only enabled when compiled on macOS with Clang.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_os_log(void);

/**
 * @test sirtest_event_log
 * @brief Properly open, configure, and send messages to the Windows event log.
 * @note Only enabled on Windows.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_win_eventlog(void);

/**
 * @test sirtest_filesystem
 * @brief Ensure the proper functionality of portable filesystem implementation.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_filesystem(void);

/**
 * @test sirtest_squelchspam
 * @brief Ensure that spam messages are squelched, normal messages are not,
 * and proper return values result from calls into libsir.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_squelchspam(void);

/**
 * @test sirtest_pluginloader
 * @brief Ensure that well-formed, valid plugins are successfully loaded, and
 * that ill-formed/incorrectly behaving plugins are rejected.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_pluginloader(void);

/**
 * @test sirtest_stringutils
 * @brief Ensure the string utility routines are functioning properly.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_stringutils(void);

/**
 * @test sirtest_getcpucount
 * @brief Ensure the processor counting routines are functioning properly.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_getcpucount(void);

/**
 * @test sirtest_getversioninfo
 * @brief Ensure that version retrieval exports are functioning properly.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_getversioninfo(void);

/**
 * @test sirtest_threadpool
 * @brief Ensure the proper functioning of the thread pool and job queue mech-
 * anisms.
 * @returns bool `true` if the test succeeded, `false` otherwise.
 */
bool sirtest_threadpool(void);

/** @} */

/**
 * @ test
 * @ returns bool `true` if the test succeeded, `false` otherwise.
bool sirtest_xxxx(void); */

# if defined(SIR_OS_LOG_ENABLED)
void os_log_parent_activity(void* ctx);
void os_log_child_activity(void* ctx);
# endif

/*
 * Utility functions, macros, and types
 */

# define INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, p_name, init) \
    sirinit var         = {0}; \
    var.d_stdout.opts   = (o_stdout) > 0 ? (o_stdout) : SIRO_DEFAULT; \
    var.d_stdout.levels = (l_stdout); \
    var.d_stderr.opts   = (o_stderr) > 0 ? (o_stderr) : SIRO_DEFAULT; \
    var.d_stderr.levels = (l_stderr); \
    if (_sir_validstrnofail(p_name)) \
        (void)_sir_strncpy(var.name, SIR_MAXNAME, (p_name), SIR_MAXNAME); \
    bool var##_init = false; \
    if (init) \
        var##_init = sir_init(&var); \
    else \
        (void)var##_init

# define INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, true)

# define INIT_SL(var, l_stdout, o_stdout, l_stderr, o_stderr, name) \
    INIT_BASE(var, l_stdout, o_stdout, l_stderr, o_stderr, name, false)

# define INIT(var, l_stdout, o_stdout, l_stderr, o_stderr) \
    INIT_N(var, l_stdout, o_stdout, l_stderr, o_stderr, "")

/**
 * Allows a specific error (identified by `err`) to be filtered out. Returns true
 * if no error occurred or if `err` is the last error that occurred on the current
 * thread; false otherwise.
 */
bool filter_error(bool pass, uint16_t err);

/**
 * If running under Wine, returns the Wine version. Returns NULL otherwise.
 */
char* get_wineversion(void);

/**
 * Used by the file-archive-large test to try multiple variations of filenames.
 */
bool roll_and_archive(const char* filename, const char* extension);

#endif /* !_SIR_TESTS_H_INCLUDED */
