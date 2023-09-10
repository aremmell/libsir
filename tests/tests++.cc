#include "tests++.hh"
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace sir;
using namespace sir::tests;

static test_vector tests = {
    {"init-cleanup-raii",   raii_init_cleanup, false, true},
    {"init-cleanup-manual", manual_init_cleanup, false, true},
    {"error-handling",      error_handling, false, true},
    {"std-format",          std_format, false, true},
    {"boost-format",        boost_format, false, true},
    {"fmt-format",          fmt_format, false, true},
    {"std-iostream-format", std_iostream_format, false, true}
};

int main(int argc, char** argv) {
    SIR_UNUSED(argc);
    SIR_UNUSED(argv);

    int retval = EXIT_SUCCESS;
    try {
        // TODO: implement CLI parser
        // TODO: consolidate main() from tests.c
        print_intro(0);

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
