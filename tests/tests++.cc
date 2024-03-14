/*
 * tests++.cc
 *
 * Version: 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
 * -----------------------------------------------------------------------------
 */

#include "tests++.hh"
#include <limits>
#include <cstdlib>
#include <cstdio>
#include <vector>

using namespace std;
using namespace sir;
using namespace sir::tests;

/** List of available tests. */
static vector<sir_test> sirxx_tests = {
    {"init-cleanup-raii",   raii_init_cleanup, false, true},
    {"init-cleanup-manual", manual_init_cleanup, false, true},
    {"error-handling",      error_handling, false, true},
    {"exception_handling",  exception_handling, false, true},
    {"format-std",          std_format, false, true},
    {"format-boost",        boost_format, false, true},
    {"format-fmt",          fmt_format, false, true},
    {"format-std-iostream", std_iostream_format, false, true}
};

/** List of available command line arguments. */
static const vector<sir_cl_arg> cl_args = {
    {SIR_CL_ONLYFLAG,      ""  SIR_CL_ONLYUSAGE, SIR_CL_ONLYDESC},
    {SIR_CL_LISTFLAG,      "", SIR_CL_LISTDESC},
    {SIR_CL_LEAVELOGSFLAG, "", SIR_CL_LEAVELOGSDESC},
    {SIR_CL_WAITFLAG,      "", SIR_CL_WAITDESC},
    {SIR_CL_VERSIONFLAG,   "", SIR_CL_VERSIONDESC},
    {SIR_CL_HELPFLAG,      "", SIR_CL_HELPDESC}
};

static sir_cl_config cl_cfg {};

int main(int argc, char** argv) {
    if (bool parsed = parse_cmd_line(argc, argv, cl_args.data(), cl_args.size(),
        sirxx_tests.data(), sirxx_tests.size(), &cl_cfg); !parsed)
        return EXIT_FAILURE;

    try {
        size_t tgt_tests = (cl_cfg.only ? cl_cfg.to_run : sirxx_tests.size());
        size_t passed    = 0;
        size_t ran       = 0;
        sir_time timer {};

        print_intro(tgt_tests);
        sir_timer_start(&timer);

        for (auto& test : sirxx_tests) {
            if (cl_cfg.only && !test.run) {
                _sir_selflog("skipping '%s'; not marked to run", test.name);
                continue;
            }

            print_test_intro(ran + 1, tgt_tests, test.name);

            if (test.pass = test.fn(); test.pass) {
                passed++;
            }

            ran++;

            print_test_outro(ran, tgt_tests, test.name, test.pass);
        }

        print_test_summary(tgt_tests, passed, sir_timer_elapsed(&timer));

        if (passed != tgt_tests) {
            print_failed_test_intro(tgt_tests, passed);

            for (const auto& test : sirxx_tests) {
                if (!test.pass) {
                    print_failed_test(test.name);
                }
            }

            (void)printf("%s", SIR_EOL);
        }

        if (cl_cfg.wait) {
            wait_for_keypress();
        }

        return passed == tgt_tests ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (const std::exception& ex) {
        ERROR_MSG("std::exception: '%s'; exiting!", ex.what());
        return EXIT_FAILURE;
    } catch (...) {
        ERROR_MSG_0("unknown exception; exiting!");
        return EXIT_FAILURE;
    }
}

bool sir::tests::raii_init_cleanup() {
    _SIR_TEST_COMMENCE

    /* scope a RAII logger object and test whether or not it successfully
     * initializes and cleans up libsir. */
    do
    {
        TEST_MSG_0("create scoped RAII logger...");
        default_logger log;

        TEST_MSG_0("scoped RAII logger created; test logging...");
        _sir_eqland(pass, log.debug("Testing debug level"));
        _sir_eqland(pass, log.info("Testing info level"));
        _sir_eqland(pass, log.notice("Testing notice level"));
        _sir_eqland(pass, log.warn("Testing warning level"));
        _sir_eqland(pass, log.error("Testing error level"));
        _sir_eqland(pass, log.crit("Testing critical level"));
        _sir_eqland(pass, log.alert("Testing alert level"));
        _sir_eqland(pass, log.emerg("Testing emergency level"));
    } while (false);

    TEST_MSG_0("confirm cleaned up...");

    /* should fail; already cleaned up. */
    _sir_eqland(pass, !sir_cleanup());

    /* create a RAII logger object that will initialize libsir, then attempt
     * to instantiate another. when using the default policy, this will result
     * in an exception being thrown. */
    try {
        TEST_MSG_0("create RAII logger...");
        default_logger log;
        TEST_MSG_0("RAII logger created; create another which should throw...");
        default_logger log2;
    } catch (std::exception& ex) {
        _SIR_TEST_ON_EXPECTED_EXCEPTION(ex.what());
        _sir_eqland(pass, _sir_validstrnofail(ex.what()));
    }

    _SIR_TEST_COMPLETE
}

bool sir::tests::manual_init_cleanup() {
    _SIR_TEST_COMMENCE

    TEST_MSG_0("manually initialize logger...");
    logger<false, default_policy, default_adapter> log;
    _sir_eqland(pass, log.init());

    TEST_MSG_0("manually initialized; initialize again...");
    _sir_eqland(pass, !log.init());

    TEST_MSG_0("test logging...");

    _sir_eqland(pass, log.debug("Testing debug level"));
    _sir_eqland(pass, log.info("Testing info level"));
    _sir_eqland(pass, log.notice("Testing notice level"));
    _sir_eqland(pass, log.warn("Testing warning level"));
    _sir_eqland(pass, log.error("Testing error level"));
    _sir_eqland(pass, log.crit("Testing critical level"));
    _sir_eqland(pass, log.alert("Testing alert level"));
    _sir_eqland(pass, log.emerg("Testing emergency level"));

    TEST_MSG_0("manually clean up logger...");
    _sir_eqland(pass, log.cleanup());

    TEST_MSG_0("clean up again...");
    _sir_eqland(pass, !log.cleanup());

    _SIR_TEST_COMPLETE
}

bool sir::tests::error_handling() {
    _SIR_TEST_COMMENCE

    /* test retrieval of libsir errors from logger::get_error() and
     * logger::get_error_info(). */
    TEST_MSG_0("create RAII logger...");
    default_logger log;

    TEST_MSG_0("RAII logger created; pass bad sirfileid to logger::rem_file...");

    try {
        _sir_eqland(pass, !log.rem_file(1234));
    } catch (sir::exception& ex) {
        _SIR_TEST_ON_EXPECTED_EXCEPTION(ex.what());
    }

    auto print_error = [&log]() {
        TEST_MSG_0("get error with logger::get_error...");
        const auto err = log.get_error();
        TEST_MSG("error: os error = %s, code = %" PRIu16 ", message = '%s'",
            (err.is_os_error() ? "yes" : "no"), err.code, err.message.c_str());
    };

    auto print_error_info = [&log]() {
        TEST_MSG_0("get error information with logger::get_error_info...");
        const auto errinfo = log.get_error_info();
        TEST_MSG("error info: os error = %s, code = %" PRIu16 ", message = '%s',"
            " function = '%s', file = '%s', line = %" PRIu32 ", OS code = %d,"
            " OS message = '%s'", (errinfo.is_os_error() ? "yes" : "no"),
            errinfo.code, errinfo.message.c_str(), errinfo.func.c_str(),
            errinfo.file.c_str(), errinfo.line, errinfo.os_code,
            errinfo.os_message.c_str());
    };

    print_error();
    print_error_info();
    _sir_eqland(pass, log.debug("ensure logging still operational"));

    TEST_MSG_0("pass invalid file name to logger::add_file...");

    try {
        _sir_eqland(pass, !log.add_file("nonexistent/file", SIRL_ALL, SIRO_DEFAULT));
    } catch (sir::exception& ex) {
        _SIR_TEST_ON_EXPECTED_EXCEPTION(ex.what());
    }

    print_error();
    print_error_info();
    _sir_eqland(pass, log.debug("ensure logging still operational"));

    _SIR_TEST_COMPLETE
}

bool sir::tests::exception_handling() {
    _SIR_TEST_COMMENCE

    try {
        TEST_MSG_0("throw a sir::exception with a string message...");
        throw exception("something has gone terribly wrong!");
    } catch (sir::exception& ex) {
        _SIR_TEST_ON_EXPECTED_EXCEPTION(ex.what());
        _sir_eqland(pass, _sir_validstrnofail(ex.what()));
    }

    try {
        default_logger log;
        TEST_MSG_0("throw a sir::exception from a libsir error...");
        _sir_eqland(pass, !log.add_file("", SIRL_NONE, SIRO_ALL));
    } catch (sir::exception& ex) {
        _SIR_TEST_ON_EXPECTED_EXCEPTION(ex.what());
        _sir_eqland(pass, _sir_validstrnofail(ex.what()));
    }

    try {
        TEST_MSG_0("throw a sir::exception from an error struct...");
        throw exception({ 1234, "a fake error"});
    } catch (sir::exception& ex) {
        _SIR_TEST_ON_EXPECTED_EXCEPTION(ex.what());
        _sir_eqland(pass, _sir_validstrnofail(ex.what()));
    }

    /* ensure that exceptions are not thrown with a policy that does not
     * enable exceptions. */
    class nothrow_policy : public default_policy {
    public:
        nothrow_policy() = default;
        ~nothrow_policy() override = default;

        static constexpr bool throw_on_error() noexcept {
            return false;
        }
    };

    TEST_MSG_0("create a logger with a no-throw policy...");
    logger<true, nothrow_policy, default_adapter> log;

    TEST_MSG_0("created; call a method that would normally throw...");
    _sir_eqland(pass, !log.load_plugin(""));
    TEST_MSG_0("if you can read this, no exception was thrown");

    _SIR_TEST_COMPLETE
}

bool sir::tests::std_format() {
    _SIR_TEST_COMMENCE

    /* std:: format. */
#if defined(__SIR_HAVE_STD_FORMAT__)
    std_format_logger log;
    _sir_eqland(pass, log.debug_std("Testing {} {}",  "std::format", "Howdy"));
    _sir_eqland(pass, log.info_std("Testing {} {}",   "std::format", true));
    _sir_eqland(pass, log.notice_std("Testing {} {}", "std::format", 1.0 / 1e9));
    _sir_eqland(pass, log.warn_std("Testing {} {}",   "std::format", std::to_string(123456789)));
    _sir_eqland(pass, log.error_std("Testing {} {}",  "std::format", std::numeric_limits<uint64_t>::max()));
    _sir_eqland(pass, log.crit_std("Testing {} {}",   "std::format", 0b10101010));
    _sir_eqland(pass, log.alert_std("Testing {} {}",  "std::format", 0x80000000U));
    _sir_eqland(pass, log.emerg_std("Testing {} {}",  "std::format", 3.14));
#else
    TEST_MSG_0(SIR_EMPH(SIR_BBLUE("std::format support not enabled; skipping")));
#endif // !__SIR_HAVE_STD_FORMAT__

    _SIR_TEST_COMPLETE
}

bool sir::tests::boost_format() {
    _SIR_TEST_COMMENCE

    /* boost::format. */
#if defined(__SIR_HAVE_BOOST_FORMAT__)
    using bf = boost::format;
    boost_logger log;
    _sir_eqland(pass, log.debug_bf(bf("Testing %1% %2%")  % "boost" % "Howdy"));
    _sir_eqland(pass, log.info_bf(bf("Testing %1% %2%")   % "boost" % true));
    _sir_eqland(pass, log.notice_bf(bf("Testing %1% %2%") % "boost" % (1.0 / 1e9)));
    _sir_eqland(pass, log.warn_bf(bf("Testing %1% %2%")   % "boost" % std::to_string(123456789)));
    _sir_eqland(pass, log.error_bf(bf("Testing %1% %2%")  % "boost" % std::numeric_limits<uint64_t>::max()));
    _sir_eqland(pass, log.crit_bf(bf("Testing %1% %2%")   % "boost" % 0b10101010));
    _sir_eqland(pass, log.alert_bf(bf("Testing %1% %2%")  % "boost" % 0x80000000U));
    _sir_eqland(pass, log.emerg_bf(bf("Testing %1% %2%")  % "boost" % 3.14));
#else
    TEST_MSG_0(SIR_EMPH(SIR_BBLUE("boost::format support not enabled; skipping")));
#endif // !__SIR_HAVE_BOOST_FORMAT__

    _SIR_TEST_COMPLETE
}

bool sir::tests::fmt_format() {
    _SIR_TEST_COMMENCE

    /* fmt */
#if defined(__SIR_HAVE_FMT_FORMAT__)
    fmt_logger log;
    _sir_eqland(pass, log.debug_fmt("Testing {} {}",  "fmt", "Howdy"));
    _sir_eqland(pass, log.info_fmt("Testing {} {}",   "fmt", true));
    _sir_eqland(pass, log.notice_fmt("Testing {} {}", "fmt", 1.0 / 1e9));
    _sir_eqland(pass, log.warn_fmt("Testing {} {}",   "fmt", std::to_string(123456789)));
    _sir_eqland(pass, log.error_fmt("Testing {} {}",  "fmt", std::numeric_limits<uint64_t>::max()));
    _sir_eqland(pass, log.crit_fmt("Testing {} {}",   "fmt", 0b10101010));
    _sir_eqland(pass, log.alert_fmt("Testing {} {}",  "fmt", 0x80000000U));
    _sir_eqland(pass, log.emerg_fmt("Testing {} {}",  "fmt", 3.14));
#else
    TEST_MSG_0(SIR_EMPH(SIR_BBLUE("fmt::format support not enabled; skipping")));
#endif // !__SIR_HAVE_FMT_FORMAT__

    _SIR_TEST_COMPLETE
}

bool sir::tests::std_iostream_format() {
    _SIR_TEST_COMMENCE

#if !defined(SIR_NO_STD_IOSTREAM)
    iostream_logger log;

    TEST_MSG_0("all levels...");

    log.debug_stream << "Testing debug level" << endl;
    _sir_eqland(pass, log.debug_stream.good());

    log.info_stream << "Testing info level" << endl;
    _sir_eqland(pass, log.info_stream.good());

    log.notice_stream << "Testing notice level" << endl;
    _sir_eqland(pass, log.notice_stream.good());

    log.warn_stream << "Testing warn level" << endl;
    _sir_eqland(pass, log.warn_stream.good());

    log.error_stream << "Testing error level" << endl;
    _sir_eqland(pass, log.error_stream.good());

    log.crit_stream << "Testing crit level" << endl;
    _sir_eqland(pass, log.crit_stream.good());

    log.alert_stream << "Testing alert level" << endl;
    _sir_eqland(pass, log.alert_stream.good());

    log.emerg_stream << "Testing emerg level" << endl;
    _sir_eqland(pass, log.emerg_stream.good());

    TEST_MSG_0("some simple, short messages...");

    log.debug_stream << "If you can see this, std::iostream is working." << flush;
    _sir_eqland(pass, log.debug_stream.good());
    log.debug_stream << "With flush as a deliminator..." << flush;
    _sir_eqland(pass, log.debug_stream.good());
    log.debug_stream << "and newlines, too." << endl;
    _sir_eqland(pass, log.debug_stream.good());

    TEST_MSG_0("a longer message...");

    log.debug_stream << "Lorem ipsum dolor sit amet, vix essent aliquid ut, "
                     << "modus propriae praesent ius ei. Ut nam sale feugait "
                     << "petentium, no quem diceret mel. At assum apeirian "
                     << "verterem qui. Ius ei illud quidam periculis. "
                     << "Etiam aliquid labores ut ius, usu et numquam docendi. "
                     << "Ne mucius quidam epicurei sea, te indoctum periculis per. "
                     << "Mel utinam latine praesent eu. Dicat consulatu inciderint "
                     << "duo ei, alterum concludaturque sit cu." << flush;
    _sir_eqland(pass, log.debug_stream.good());

    TEST_MSG_0("ostream::write()...");

    const auto str = std::string("This is my string. There are many like it, but "
        "this one is mine. I'm going to ostream::write it now.");
    log.debug_stream.write(str.c_str(), str.size()) << endl;
    _sir_eqland(pass, log.debug_stream.good());

    TEST_MSG_0("overflow internal buffer...");
    array<char, SIR_MAXMESSAGE + 100> large_buffer {};
    large_buffer.fill('a');
    large_buffer.data()[large_buffer.size() - 1] = '\0';

    log.info_stream << large_buffer.data() << endl;
    _sir_eqland(pass, log.info_stream.good());
#else
    TEST_MSG_0(SIR_EMPH(SIR_BBLUE("std::iostream support not enabled; skipping")));
#endif

    _SIR_TEST_COMPLETE
}
