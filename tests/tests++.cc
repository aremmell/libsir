/*
 * tests++.cc
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
#include "tests++.hh"
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace sir;
using namespace sir::tests;

/** List of available tests. */
static sir_test sirxx_tests[] = {
    {"init-cleanup-raii",   raii_init_cleanup, false, true},
    {"init-cleanup-manual", manual_init_cleanup, false, true},
    {"error-handling",      error_handling, false, true},
    {"format-std",          std_format, false, true},
    {"format-boost",        boost_format, false, true},
    {"format-fmt",          fmt_format, false, true},
    {"format-std-iostream", std_iostream_format, false, true}
};

/** List of available command line arguments. */
static const sir_cl_arg cl_args[] = {
    {SIR_CL_ONLYFLAG,      ""  SIR_CL_ONLYUSAGE, SIR_CL_ONLYDESC},
    {SIR_CL_LISTFLAG,      "", SIR_CL_LISTDESC},
    {SIR_CL_LEAVELOGSFLAG, "", SIR_CL_LEAVELOGSDESC},
    {SIR_CL_WAITFLAG,      "", SIR_CL_WAITDESC},
    {SIR_CL_VERSIONFLAG,   "", SIR_CL_VERSIONDESC},
    {SIR_CL_HELPFLAG,      "", SIR_CL_HELPDESC}
};

static sir_cl_config cl_cfg {};

int main(int argc, char** argv) {
    if (!parse_cmd_line(argc, argv, cl_args, _sir_countof(cl_args),
        sirxx_tests, _sir_countof(sirxx_tests), &cl_cfg))
        return EXIT_FAILURE;

    int retval = EXIT_SUCCESS;
    try {
        // TODO: consolidate main() from tests.c
        print_intro(0);

        if (cl_cfg.wait)
            wait_for_keypress();
    } catch (const std::exception& ex) {
        ERROR_MSG("std::exception: '%s'; exiting!", ex.what());
        retval = EXIT_FAILURE;
    } catch (...) {
        ERROR_MSG_0("unknown exception; exiting!");
        retval = EXIT_FAILURE;
    }

    return retval;
}

bool sir::tests::raii_init_cleanup() {
    _SIR_TEST_BEGIN

    /* scope a RAII logger object and test whether or not it successfully
     * initializes and cleans up libsir. */
    do
    {
        default_logger log;

        _sir_eqland(pass, log.debug("C++ RAII logger initialized via constructor"));
        _sir_eqland(pass, log.info("Testing info level"));
        _sir_eqland(pass, log.notice("Testing notice level"));
        _sir_eqland(pass, log.warn("Testing warning level"));
        _sir_eqland(pass, log.error("Testing error level"));
        _sir_eqland(pass, log.crit("Testing critical level"));
        _sir_eqland(pass, log.alert("Testing alert level"));
        _sir_eqland(pass, log.emerg("Testing emergency level"));
        _sir_eqland(pass, log.debug("Cleanup will be performed via destructor"));
    } while (false);

    _sir_eqland(pass, !sir_cleanup());

    _SIR_TEST_END
}

bool sir::tests::manual_init_cleanup() {
    _SIR_TEST_BEGIN

    logger<false, default_init_policy, default_adapter> log;
    _sir_eqland(pass, log.init());

    _sir_eqland(pass, log.debug("Manually initialized using default_init_policy"));
    _sir_eqland(pass, log.debug("Will not clean up in the destructor; cleaning up manually"));

    _sir_eqland(pass, log.cleanup());

    _SIR_TEST_END
}

bool sir::tests::error_handling() {
    _SIR_TEST_BEGIN

    default_logger log;

    _sir_eqland(pass, log.debug("Testing get_error by doing something stupid..."));
    _sir_eqland(pass, !log.debug(nullptr));

    const auto [ code, message ] = log.get_error();
    _sir_eqland(pass, log.debug("Error info: code = %u, message = '%s'", code,
        message.c_str()));

    _SIR_TEST_END
}

bool sir::tests::std_format() {
    _SIR_TEST_BEGIN

    /* std:: format. */
#if defined(__SIR_HAVE_STD_FORMAT__)
    default_logger log;
    _sir_eqland(pass, log.debug_std("Testing {} {}", "debug", "std::format"));
    _sir_eqland(pass, log.info_std("Testing {} {}", "info", "std::format"));
    _sir_eqland(pass, log.notice_std("Testing {} {}", "notice", "std::format"));
    _sir_eqland(pass, log.warn_std("Testing {} {}", "warn", "std::format"));
    _sir_eqland(pass, log.error_std("Testing {} {}", "error", "std::format"));
    _sir_eqland(pass, log.crit_std("Testing {} {}", "crit", "std::format"));
    _sir_eqland(pass, log.alert_std("Testing {} {}", "alert", "std::format"));
    _sir_eqland(pass, log.emerg_std("Testing {} {}", "emerg", "std::format"));
#else
    TEST_MSG_0("std::format support not enabled; skipping");
    pass = true;
#endif // !__SIR_HAVE_STD_FORMAT__

    _SIR_TEST_END
}

bool sir::tests::boost_format() {
    _SIR_TEST_BEGIN

    /* boost::format. */
#if defined(__SIR_HAVE_BOOST_FORMAT__)
    default_logger log;
    _sir_eqland(pass, log.debug_boost(boost::format("Testing %1% %2%") % "debug" % "boost::format"));
    _sir_eqland(pass, log.info_boost(boost::format("Testing %1% %2%") % "info" % "boost::format"));
    _sir_eqland(pass, log.notice_boost(boost::format("Testing %1% %2%") % "notice" % "boost::format"));
    _sir_eqland(pass, log.warn_boost(boost::format("Testing %1% %2%") % "warn" % "boost::format"));
    _sir_eqland(pass, log.error_boost(boost::format("Testing %1% %2%") % "error" % "boost::format"));
    _sir_eqland(pass, log.crit_boost(boost::format("Testing %1% %2%") % "crit" % "boost::format"));
    _sir_eqland(pass, log.alert_boost(boost::format("Testing %1% %2%}") % "alert" % "boost::format"));
    _sir_eqland(pass, log.emerg_boost(boost::format("Testing %1% %2%") % "emerg" % "boost::format"));
#else
    TEST_MSG_0("boost::format support not enabled; skipping");
    pass = true;
#endif // !__SIR_HAVE_BOOST_FORMAT__

    _SIR_TEST_END
}

bool sir::tests::fmt_format() {
    _SIR_TEST_BEGIN

    /* fmt */
#if defined(__SIR_HAVE_FMT_FORMAT__)
    default_logger log;
    _sir_eqland(pass, log.debug_fmt("Testing {} {}", "debug", "fmt"));
    _sir_eqland(pass, log.info_fmt("Testing {} {}", "info", "fmt"));
    _sir_eqland(pass, log.notice_fmt("Testing {} {}", "notice", "fmt"));
    _sir_eqland(pass, log.warn_fmt("Testing {} {}", "warn", "fmt"));
    _sir_eqland(pass, log.error_fmt("Testing {} {}", "error", "fmt"));
    _sir_eqland(pass, log.crit_fmt("Testing {} {}", "crit", "fmt"));
    _sir_eqland(pass, log.alert_fmt("Testing {} {}", "alert", "fmt"));
    _sir_eqland(pass, log.emerg_fmt("Testing {} {}", "emerg", "fmt"));
#else
    TEST_MSG_0("fmt::format support not enabled; skipping");
    pass = true;
#endif // !__SIR_HAVE_FMT_FORMAT__

    _SIR_TEST_END
}

bool sir::tests::std_iostream_format() {
    _SIR_TEST_BEGIN

    default_logger log;

    log.streams.debug << "ABCDEF";
    log.streams.debug << "GHIJKLMNOPQRSTUVWXYZ" << 1234 << "\n";

    _SIR_TEST_END
}
