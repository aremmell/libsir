/*
 * tests++.hh
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
#ifndef _TESTSXX_HH_INCLUDED
# define _TESTSXX_HH_INCLUDED

# include "sir.hh"
# include "tests_shared.h"

/**
 * @addtogroup tests
 * @{
 */

/** Contains the C++ tests and test rig implementation. */
namespace sir::tests
{
    /**
     * @test raii_init_cleanup
     * @brief Ensure that RAII loggers are functioning properly.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool raii_init_cleanup();

    /**
     * @test manual_init_cleanup
     * @brief Ensure that manually initializing and cleaning up is working properly.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool manual_init_cleanup();

    /**
     * @test error_handling
     * @brief Ensure that the C++ wrapper properly handles errors from libsir.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool error_handling();

    /**
     * @test std_format
     * @brief If std::format support is enabled, ensures that it is working properly.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool std_format();

    /**
     * @test boost_format
     * @brief If boost::format support is enabled, ensures that it is working properly.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool boost_format();

    /**
     * @test fmt_format
     * @brief If fmt::format support is enabled, ensures that it is working properly.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool fmt_format();

    /**
     * @test std_iostream_format
     * @brief If std::iostream support is enabled, ensures that it is working properly.
     * @returns `true` if the test succeeded, `false` otherwise.
     */
    bool std_iostream_format();

    /**
     * @ test
     * @ brief
     * @ returns `true` if the test succeeded, `false` otherwise.
     */
    //bool ();
} // !namespace sir::tests

/* @} */

/**
 * Macros
 */

/** Begins a test by declaring `pass` and entering a try/catch block. */
# define _SIR_TEST_BEGIN \
    bool pass = true; \
    try {

/** Implements recovery in the event that an exception is caught. */
# define _SIR_ON_TEST_EXCEPTION(excpt) \
    ERROR_MSG("exception in %s: '%s'", __PRETTY_FUNCTION__, excpt); \
    pass = false; \
    (void)sir_cleanup()

/** Ends a test by closing the try/catch block and implementing exception handling. */
# define _SIR_TEST_END \
    } catch (const std::exception& ex) { \
        _SIR_ON_TEST_EXCEPTION(ex.what()); \
    } catch (...) { \
        _SIR_ON_TEST_EXCEPTION(SIR_UNKNOWN); \
    } \
    return pass;

#endif // !_TESTSXX_HH_INCLUDED
