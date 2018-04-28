#include <stdio.h>
#include <stdlib.h>
#include "sir.h"

int main(int argc, char** argv) {

    sirinit si = {0};

    si.processName = "jason.nig";

    si.stdOutLevels = SIRL_ALL;
    si.stdOutOptions = SIRO_NOMSEC;

    if (!sir_init(&si)) {
        fprintf(stderr, "Failed to initialize SIR!\n");
        return 1;
    }

    sir_options file1opts = 0;
    int id1 = sir_addfile("test.log", SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file 1!\n");
        return 1;
    }

    for (size_t j = 0; j < 10; j++) {
        for (size_t n = 0; n < 100; n++) {
            sirdebug("debug message: %d", (n*j)+n);
            sirinfo("info message: %d", (n*j)+n);
            sirnotice("notice message: %d", (n*j)+n);
            sirwarn("warning message: %d", (n*j)+n);
            sirerror("error message: %d", (n*j)+n);
            sircrit("critical message: %d", (n*j)+n);
            siralert("alert message: %d", (n*j)+n);
            siremerg("emergency message: %d", (n*j)+n);
        }
    }

    sir_cleanup();

    return 0;
}
