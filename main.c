#include <stdio.h>
#include <stdlib.h>

#include "sir.h"

int main(int argc, char** argv) {

    sirinit si = {0};

    si.processName = "jason.nig";
    si.stdOutLevels = SIRL_ALL;

    if (!sir_init(&si)) {
        fprintf(stderr, "Failed to initialize SIR!\n");
        return 1;
    }

    sirdebug("lorem ipsum foo bar, lorem ipsum foo bar %s, %d",
        "sups", 12345);

    sirdebug("lorem ipsum foo bar, lorem ipsum foo bar, lorem ipsum foo bar, lorem ipsum foo bar %s, %d",
    "sups", 12345);

    sirdebug("hi %s, %d",
        "sups", 12345);


    /*sir_options file1opts = 0;
    int id1 = sir_addfile("test.log", SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file 1!\n");
        return 1;
    }

    sirdebug("debug message: %d", 123);

    sirinfo("info message: %d", 123);

    sirnotice("notice message: %d", 123);
    
    sirwarn("warning message: %d", 123);

     // remove file and continue
    if (!sir_remfile(id1)) {
        fprintf(stderr, "Failed to remove file 1!\n");
    }

    sirerror("error message: %d", 123);

    sircrit("critical message: %d", 123);

    siralert("alert message: %d", 123);

    siremerg("emergency message: %d", 123);*/

    sir_cleanup();

    return 0;
}
