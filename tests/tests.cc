#include "sir.hh"
#include <cstdlib>

using namespace std;
using namespace sir;

int main(int argc, char** argv) {
    SIR_UNUSED(argc);
    SIR_UNUSED(argv);

    default_log log;

    log.debug("C++ RAII log initialized via constructor");
    log.info("Testing info level");
    log.notice("Testing notice level");
    log.warn("Testing warning level");
    log.error("Testing error level");
    log.crit("Testing critical level");
    log.alert("Testing alert level");
    log.emerg("Testing emergency level");

    log.debug("Testing get_error by doing something stupid...");
    log.debug(NULL);
    auto [ code, message ] = log.get_error();

    log.debug("Error info: code = %u, message = '%s'", code,
        message.c_str());

    log.debug("sir_cleanup() will be called by the destructor");

    return EXIT_SUCCESS;
}
