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
    bool parsed = parse_cmd_line(argc, argv, cl_args, _sir_countof(cl_args),
        sirxx_tests, _sir_countof(sirxx_tests), &cl_cfg);
    if (!parsed)
        return EXIT_FAILURE;

    try {
        size_t first     = 0;
        size_t tgt_tests = (cl_cfg.only ? cl_cfg.to_run : _sir_countof(sirxx_tests));
        size_t passed    = 0;
        size_t ran       = 0;
        sir_time timer {};

        print_intro(tgt_tests);
        sir_timer_start(&timer);

        for (size_t n = first; n < _sir_countof(sirxx_tests); n++) {
            if (cl_cfg.only && !sirxx_tests[n].run) {
                _sir_selflog("skipping '%s'; not marked to run", sirxx_tests[n].name);
                continue;
            }

            print_test_intro(ran + 1, tgt_tests, sirxx_tests[n].name);

            if (sirxx_tests[n].pass = sirxx_tests[n].fn(); sirxx_tests[n].pass)
                passed++;

            ran++;

            print_test_outro(ran, tgt_tests, sirxx_tests[n].name, sirxx_tests[n].pass);
        }

        print_test_summary(tgt_tests, passed, sir_timer_elapsed(&timer));

        if (passed != tgt_tests) {
            print_failed_test_intro(tgt_tests, passed);

            for (size_t t = 0; t < _sir_countof(sirxx_tests); t++)
                if (!sirxx_tests[t].pass)
                    print_failed_test(sirxx_tests[t].name);

            (void)printf("\n");
        }

        if (cl_cfg.wait)
            wait_for_keypress();

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

     /* should fail; already cleaned up. */
    _sir_eqland(pass, !sir_cleanup());

    _SIR_TEST_COMPLETE
}

bool sir::tests::manual_init_cleanup() {
    _SIR_TEST_COMMENCE

    logger<false, default_adapter> log;
    _sir_eqland(pass, log.init());

    _sir_eqland(pass, log.debug("Manually initialized"));
    _sir_eqland(pass, log.debug("Will not clean up in the destructor; cleaning up manually"));

    _sir_eqland(pass, log.cleanup());

    _SIR_TEST_COMPLETE
}

bool sir::tests::error_handling() {
    _SIR_TEST_COMMENCE

    default_logger log;

    _sir_eqland(pass, log.debug("Testing get_error by doing something stupid..."));
    _sir_eqland(pass, !log.debug(nullptr));

    const auto [ code, message ] = log.get_error();
    _sir_eqland(pass, log.debug("Error info: code = %u, message = '%s'", code,
        message.c_str()));

    _SIR_TEST_COMPLETE
}

bool sir::tests::std_format() {
    _SIR_TEST_COMMENCE

    /* std:: format. */
#if defined(__SIR_HAVE_STD_FORMAT__)
    default_logger log;
    _sir_eqland(pass, log.debug_std("Testing {} {}",  "std::format", "Howdy"));
    _sir_eqland(pass, log.info_std("Testing {} {}",   "std::format", true));
    _sir_eqland(pass, log.notice_std("Testing {} {}", "std::format", 1.0 / 1e9));
    _sir_eqland(pass, log.warn_std("Testing {} {}",   "std::format", std::to_string(123456789)));
    _sir_eqland(pass, log.error_std("Testing {} {}",  "std::format", std::numeric_limits<uint64_t>::max()));
    _sir_eqland(pass, log.crit_std("Testing {} {}",   "std::format", 0b10101010));
    _sir_eqland(pass, log.alert_std("Testing {} {}",  "std::format", 0x80000000U));
    _sir_eqland(pass, log.emerg_std("Testing {} {}",  "std::format", 3.14));
#else
    TEST_MSG_0(EMPH(BBLUE("std::format support not enabled; skipping")));
    pass = true;
#endif // !__SIR_HAVE_STD_FORMAT__

    _SIR_TEST_COMPLETE
}

bool sir::tests::boost_format() {
    _SIR_TEST_COMMENCE

    /* boost::format. */
#if defined(__SIR_HAVE_BOOST_FORMAT__)
    using bf = boost::format;
    default_logger log;
    _sir_eqland(pass, log.debug_boost(bf("Testing %1% %2%")  % "boost" % "Howdy"));
    _sir_eqland(pass, log.info_boost(bf("Testing %1% %2%")   % "boost" % true));
    _sir_eqland(pass, log.notice_boost(bf("Testing %1% %2%") % "boost" % (1.0 / 1e9)));
    _sir_eqland(pass, log.warn_boost(bf("Testing %1% %2%")   % "boost" % std::to_string(123456789)));
    _sir_eqland(pass, log.error_boost(bf("Testing %1% %2%")  % "boost" % std::numeric_limits<uint64_t>::max()));
    _sir_eqland(pass, log.crit_boost(bf("Testing %1% %2%")   % "boost" % 0b10101010));
    _sir_eqland(pass, log.alert_boost(bf("Testing %1% %2%")  % "boost" % 0x80000000U));
    _sir_eqland(pass, log.emerg_boost(bf("Testing %1% %2%")  % "boost" % 3.14));
#else
    TEST_MSG_0(EMPH(BBLUE("boost::format support not enabled; skipping")));
    pass = true;
#endif // !__SIR_HAVE_BOOST_FORMAT__

    _SIR_TEST_COMPLETE
}

bool sir::tests::fmt_format() {
    _SIR_TEST_COMMENCE

    /* fmt */
#if defined(__SIR_HAVE_FMT_FORMAT__)
    default_logger log;
    _sir_eqland(pass, log.debug_fmt("Testing {} {}",  "fmt", "Howdy"));
    _sir_eqland(pass, log.info_fmt("Testing {} {}",   "fmt", true));
    _sir_eqland(pass, log.notice_fmt("Testing {} {}", "fmt", 1.0 / 1e9));
    _sir_eqland(pass, log.warn_fmt("Testing {} {}",   "fmt", std::to_string(123456789)));
    _sir_eqland(pass, log.error_fmt("Testing {} {}",  "fmt", std::numeric_limits<uint64_t>::max()));
    _sir_eqland(pass, log.crit_fmt("Testing {} {}",   "fmt", 0b10101010));
    _sir_eqland(pass, log.alert_fmt("Testing {} {}",  "fmt", 0x80000000U));
    _sir_eqland(pass, log.emerg_fmt("Testing {} {}",  "fmt", 3.14));
#else
    TEST_MSG_0(EMPH(BBLUE("fmt::format support not enabled; skipping")));
    pass = true;
#endif // !__SIR_HAVE_FMT_FORMAT__

    _SIR_TEST_COMPLETE
}

bool sir::tests::std_iostream_format() {
    _SIR_TEST_COMMENCE

#if !defined(SIR_NO_STD_IOSTREAM)
    default_logger log;

    TEST_MSG_0("all levels...");

    log.debug_stream << "Testing debug level" << endl;
    pass &= log.debug_stream.good();

    log.info_stream << "Testing info level" << endl;
    pass &= log.info_stream.good();

    log.notice_stream << "Testing notice level" << endl;
    pass &= log.notice_stream.good();

    log.warn_stream << "Testing warn level" << endl;
    pass &= log.warn_stream.good();

    log.error_stream << "Testing error level" << endl;
    pass &= log.error_stream.good();

    log.crit_stream << "Testing crit level" << endl;
    pass &= log.crit_stream.good();

    log.alert_stream << "Testing alert level" << endl;
    pass &= log.alert_stream.good();

    log.emerg_stream << "Testing emerg level" << endl;
    pass &= log.emerg_stream.good();

    TEST_MSG_0("some simple, short messages...");

    log.debug_stream << "If you can see this, std::iostream is working." << flush;
    pass &= log.debug_stream.good();
    log.debug_stream << "With flush as a deliminator," << flush;
    pass &= log.debug_stream.good();
    log.debug_stream << "and newlines, too." << endl;
    pass &= log.debug_stream.good();

    TEST_MSG_0("a longer message...");

    log.debug_stream << "Lorem ipsum dolor sit amet, vix essent aliquid ut, "
                     << "modus propriae praesent ius ei. Ut nam sale feugait "
                     << "petentium, no quem diceret mel. At assum apeirian "
                     << "verterem qui. Ius ei illud quidam periculis. "
                     << "Etiam aliquid labores ut ius, usu et numquam docendi. "
                     << "Ne mucius quidam epicurei sea, te indoctum periculis per. "
                     << "Mel utinam latine praesent eu. Dicat consulatu inciderint "
                     << "duo ei, alterum concludaturque sit cu." << flush;
    pass &= log.debug_stream.good();

    TEST_MSG_0("ostream::write()...");

    auto str = std::string("This is my string. There are many like it, but "
        "this one is mine. I'm going to ostream::write it now.");
    log.debug_stream.write(str.c_str(), str.size()) << endl;
    pass &= log.debug_stream.good();
#else
    TEST_MSG_0(EMPH(BBLUE("std::iostream support not enabled; skipping")));
    pass = true;
#endif

    _SIR_TEST_COMPLETE
}
