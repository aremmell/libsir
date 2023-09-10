#include "tests++.hh"
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace sir;
using namespace sir::tests;

static const test_vector tests = {
    {raii_init_cleanup},
    {manual_init_cleanup},
    {error_handling},
    {std_format},
    {boost_format},
    {fmt_format},
    {std_iostream_format}
};

int main(int argc, char** argv) {
    SIR_UNUSED(argc);
    SIR_UNUSED(argv);

    try {
        default_logger logger;
        
        /* logger.test_stream << "ABCDEF";
        logger.test_stream << "GHIJKLMNOPQRSTUVWXYZ" << 1234 << EOF; */

    } catch (std::exception& ex) {
        printf("ERROR: caught std::exception: '%s'!\n", ex.what());
        return EXIT_FAILURE;
    } catch (...) {
        printf("ERROR: caught unknown exception!\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

bool sir::tests::raii_init_cleanup() {
    bool pass = true;

    /* scope a RAII logger object and test whether or not it successfully
     * initializes and cleans up libsir. */
    if (true)
    {
        default_logger log;

        pass &= log.debug("C++ RAII logger initialized via constructor");
        pass &= log.info("Testing info level");
        pass &= log.notice("Testing notice level");
        pass &= log.warn("Testing warning level");
        pass &= log.error("Testing error level");
        pass &= log.crit("Testing critical level");
        pass &= log.alert("Testing alert level");
        pass &= log.emerg("Testing emergency level");
        pass &= log.debug("Cleanup will be performed via destructor");
    }

    pass &= !sir_cleanup();

    return pass;
}

bool sir::tests::manual_init_cleanup() {
    bool pass = true;

    logger<false, default_init_policy, default_adapter> log;
    pass &= log.init();

    pass &=log.debug("Manually initialized using default_init_policy");
    pass &=log.debug("Will not clean up in the destructor; cleaning up manually");

    pass &= log.cleanup();

    return pass;
}

bool sir::tests::error_handling() {
    bool pass = true;

    default_logger log;

    pass &= log.debug("Testing get_error by doing something stupid...");
    pass &= !log.debug(nullptr);

    const auto [ code, message ] = log.get_error();
    pass &= log.debug("Error info: code = %u, message = '%s'", code,
        message.c_str());

    return pass;
}

bool sir::tests::std_format() {
    bool pass = true;

    /* std:: format. */
#if defined(__SIR_HAVE_STD_FORMAT__)
    default_logger log;
    log.debug_std("Testing {} {}", "debug", "std::format");
    log.info_std("Testing {} {}", "info", "std::format");
    log.notice_std("Testing {} {}", "notice", "std::format");
    log.warn_std("Testing {} {}", "warn", "std::format");
    log.error_std("Testing {} {}", "error", "std::format");
    log.crit_std("Testing {} {}", "crit", "std::format");
    log.alert_std("Testing {} {}", "alert", "std::format");
    log.emerg_std("Testing {} {}", "emerg", "std::format");
#else
    printf("\tstd::format support not enabled; skipping\n");
    pass = true;
#endif // !__SIR_HAVE_STD_FORMAT__

    return pass;
}

bool sir::tests::boost_format() {
    bool pass = true;

    /* boost::format. */
#if defined(__SIR_HAVE_BOOST_FORMAT__)
    default_logger log;
    log.debug_boost(boost::format("Testing %1% %2%") % "debug" % "boost::format");
    log.info_boost(boost::format("Testing %1% %2%") % "info" % "boost::format");
    log.notice_boost(boost::format("Testing %1% %2%") % "notice" % "boost::format");
    log.warn_boost(boost::format("Testing %1% %2%") % "warn" % "boost::format");
    log.error_boost(boost::format("Testing %1% %2%") % "error" % "boost::format");
    log.crit_boost(boost::format("Testing %1% %2%") % "crit" % "boost::format");
    log.alert_boost(boost::format("Testing %1% %2%}") % "alert" % "boost::format");
    log.emerg_boost(boost::format("Testing %1% %2%") % "emerg" % "boost::format");
#else
    printf("\tboost::format support not enabled; skipping\n");
    pass = true;
#endif // !__SIR_HAVE_BOOST_FORMAT__

    return pass;
}

bool sir::tests::fmt_format() {
    bool pass = true;

    /* fmt */
#if defined(__SIR_HAVE_FMT_FORMAT__)
    default_logger log;
    log.debug_fmt("Testing {} {}", "debug", "fmt");
    log.info_fmt("Testing {} {}", "info", "fmt");
    log.notice_fmt("Testing {} {}", "notice", "fmt");
    log.warn_fmt("Testing {} {}", "warn", "fmt");
    log.error_fmt("Testing {} {}", "error", "fmt");
    log.crit_fmt("Testing {} {}", "crit", "fmt");
    log.alert_fmt("Testing {} {}", "alert", "fmt");
    log.emerg_fmt("Testing {} {}", "emerg", "fmt");
#else
    printf("\tfmt::format support not enabled; skipping\n");
    pass = true;
#endif // !__SIR_HAVE_FMT_FORMAT__

    return pass;
}

bool sir::tests::std_iostream_format() {
    bool pass = true;

    return pass;
}
