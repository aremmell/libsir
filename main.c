#include "sir.h"

int main(int argc, char** argv) {

    sirinit si = {0};

    strcpy(si.processName, "sir");

    si.d_stdout.levels = SIRL_ALL;
    si.d_stdout.opts   = 0;

    if (!sir_init(&si)) {
        fprintf(stderr, "Failed to initialize SIR!\n");
        return 1;
    }

    sir_options file1opts = SIRO_MSGONLY;
    int         id1       = sir_addfile("test.log", SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file 1!\n");
        return 1;
    }

    sir_notice("notice message: %d", 123);
    sir_warn("warning message: %d", 123);
    sir_error("error message: %d", 123);
    sir_crit("critical message: %d", 123);
    sir_alert("alert message: %d", 123);
    sir_emerg("emergency message: %d", 123);

    sir_cleanup();

    return 0;
}
