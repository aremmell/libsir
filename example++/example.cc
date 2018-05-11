#include "../sir.hh"

using namespace std;
using namespace sir;

int main(int argc, char** argv) {
    
    sirinit si = {0};
    si.d_stdout.levels = SIRL_ALL;

    if (!sir_init(&si))
        return 1;

    logger log;
    log.debug << "Sups bruv?" << endl;  
    log.info << "This goes to info, and shit: " << 12345 << endl;

    sir_cleanup();
    return 0;
}
