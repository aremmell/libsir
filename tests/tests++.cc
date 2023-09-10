#include "sir.hh"
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace sir;

int main(int argc, char** argv) {
    SIR_UNUSED(argc);
    SIR_UNUSED(argv);

    {
        default_logger logger;

        logger.debug("C++ RAII logger initialized via constructor");
        logger.info("Testing info level");
        logger.notice("Testing notice level");
        logger.warn("Testing warning level");
        logger.error("Testing error level");
        logger.crit("Testing critical level");
        logger.alert("Testing alert level");
        logger.emerg("Testing emergency level");

        logger.debug("Testing get_error by doing something stupid...");
        logger.debug(NULL);

        auto [ code, message ] = logger.get_error();
        logger.debug("Error info: code = %u, message = '%s'", code,
            message.c_str());

        /* std:: format. */
#if defined(__SIR_HAVE_STD_FORMAT__)
        logger.debug_std("Testing {} {}", "debug", "std::format");
        logger.info_std("Testing {} {}", "info", "std::format");
        logger.notice_std("Testing {} {}", "notice", "std::format");
        logger.warn_std("Testing {} {}", "warn", "std::format");
        logger.error_std("Testing {} {}", "error", "std::format");
        logger.crit_std("Testing {} {}", "crit", "std::format");
        logger.alert_std("Testing {} {}", "alert", "std::format");
        logger.emerg_std("Testing {} {}", "emerg", "std::format");
#endif // !__SIR_HAVE_STD_FORMAT__

        /* boost::format. */
#if defined(__SIR_HAVE_BOOST_FORMAT__)
        logger.debug_boost(boost::format("Testing %1% %2%") % "debug" % "boost::format");
        logger.info_boost(boost::format("Testing %1% %2%") % "info" % "boost::format");
        logger.notice_boost(boost::format("Testing %1% %2%") % "notice" % "boost::format");
        logger.warn_boost(boost::format("Testing %1% %2%") % "warn" % "boost::format");
        logger.error_boost(boost::format("Testing %1% %2%") % "error" % "boost::format");
        logger.crit_boost(boost::format("Testing %1% %2%") % "crit" % "boost::format");
        logger.alert_boost(boost::format("Testing %1% %2%}") % "alert" % "boost::format");
        logger.emerg_boost(boost::format("Testing %1% %2%") % "emerg" % "boost::format");
#endif // !__SIR_HAVE_BOOST_FORMAT__

        /* fmt */
#if defined(__SIR_HAVE_FMT_FORMAT__)
        logger.debug_fmt("Testing {} {}", "debug", "fmt");
        logger.info_fmt("Testing {} {}", "info", "fmt");
        logger.notice_fmt("Testing {} {}", "notice", "fmt");
        logger.warn_fmt("Testing {} {}", "warn", "fmt");
        logger.error_fmt("Testing {} {}", "error", "fmt");
        logger.crit_fmt("Testing {} {}", "crit", "fmt");
        logger.alert_fmt("Testing {} {}", "alert", "fmt");
        logger.emerg_fmt("Testing {} {}", "emerg", "fmt");
#endif // !__SIR_HAVE_FMT_FORMAT__

        logger.debug("sir_cleanup() will be called by the destructor");
    }

    {
        sir::logger<false, default_init_policy, default_adapter> logger2;
        if (!logger2.init())
            SIR_ASSERT(false);

        logger2.debug("Manually initialized using default_init_policy");
        logger2.debug("Will not clean up in the destructor; cleaning up manually");

        if (!logger2.cleanup())
            SIR_ASSERT(false);
    }


    return EXIT_SUCCESS;
}
