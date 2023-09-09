#include "sir.hh"
#include <cstdlib>

using namespace std;
using namespace sir;

int main(int argc, char** argv) {
    SIR_UNUSED(argc);
    SIR_UNUSED(argv);

    default_logger log;

    log.debug("Should have been initialized by ctor...");

    return EXIT_SUCCESS;
}
