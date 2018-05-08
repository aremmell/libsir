/**
 * @file tests.h
 * @brief Definitions for test rig app.
 */
#ifndef _SIR_TESTS_H_INCLUDED
#define _SIR_TESTS_H_INCLUDED

#include <stdbool.h>

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
 * Integrity tests for the library.
 * 
 * @addtogroup tests
 * @{
 */

/**
 * @test Properly handle multiple threads competing for locked sections.
 */
bool sirtest_mthread_race();

/**
 * @test Properly handle messages that exceed internal buffer sizes.
 */
bool sirtest_exceedmaxsize();

/**
 * @test Properly handle adding and removing log files.
 */
bool sirtest_addremovefiles();

/**
 * @test Properly handle invalid text style.
 */
bool sirtest_failsetinvalidstyle();

/**
 * @test Properly handle the lack of any output destinations.
 */
bool sirtest_failnooutputdest();

/**
 * @test Properly handle invalid log file name.
 */
bool sirtest_failinvalidfilename();

/**
 * @test Properly handle log file without appropriate permissions.
 */
bool sirtest_failfilebadpermission();

/**
 * @test Properly handle null/empty input.
 */
bool sirtest_failnulls();

/**
 * @test Properly handle calls without initialization.
 */
bool sirtest_failwithoutinit();

/**
 * @test Properly handle calls after cleanup.
 */
bool sirtest_failaftercleanup();

/**
 * @test Properly handle initialization, cleanup, re-initialization.
 */
bool sirtest_initcleanupinit();

/**
 * @test Properly refuse to add a duplicate file.
 */
bool sirtest_faildupefile();

/**
 * @test Properly refuse to remove a file that isn't added.
 */
bool sirtest_failremovebadfile();

/**
 * @test Properly roll/archive a file when it hits max size.
 */
bool sirtest_rollandarchivefile();

/**
 * @test Properly return valid codes and messages for all possible errors.
 */
bool sirtest_allerrorsresolve();

/** @} */

bool printerror(bool pass);

#endif /* !_SIR_TESTS_H_INCLUDED */
