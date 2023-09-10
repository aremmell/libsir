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
        logger.debug_format("Testing {} {}", "debug", "std::format");
        logger.info_format("Testing {} {}", "info", "std::format");
        logger.notice_format("Testing {} {}", "notice", "std::format");
        logger.warn_format("Testing {} {}", "warn", "std::format");
        logger.error_format("Testing {} {}", "error", "std::format");
        logger.crit_format("Testing {} {}", "crit", "std::format");
        logger.alert_format("Testing {} {}", "alert", "std::format");
        logger.emerg_format("Testing {} {}", "emerg", "std::format");

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
