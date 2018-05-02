#include "sir.h"
#include "tests.h"

int main(int argc, char** argv) {

    sirinit si = {0};

    strcpy(si.processName, "sir");

    si.d_stdout.levels = SIRL_ALL;
    si.d_stdout.opts = 0;

    if (!sir_init(&si)) {
        fprintf(stderr, "Failed to initialize SIR!\n");
        return 1;
    }

    int test = sirtest_mthread_race();

    sirdebug("debug message: %d", 123);
    sirinfo("info message: %d", 123);

/*     if (!sir_settextstyle(SIRL_NOTICE, SIRS_NONE))
        fprintf(stderr, "Failed to set style for SIRL_NOTICE!\n"); */


    sir_options file1opts = SIRO_MSGONLY;
    int         id1       = sir_addfile("test.log", SIRL_ALL, file1opts);

    if (SIR_INVALID == id1) {
        fprintf(stderr, "Failed to add file 1!\n");
        return 1;
    }

    sirnotice("notice message: %d", 123);
    sirwarn("warning message: %d", 123);
    sirerror("error message: %d", 123);
    sircrit("critical message: %d", 123);
    siralert("alert message: %d", 123);
    siremerg("emergency message: %d", 123);        

    for (size_t j = 0; j < 100; j++) {
        for (size_t n = 0; n < 100; n++) {
            sirdebug("debug message: %d", (n * j) + n);
            sirinfo("info message: %d", (n * j) + n);
            sirnotice("notice message: %d", (n * j) + n);
            sirwarn("warning message: %d", (n * j) + n);
            sirerror("error message: %d", (n * j) + n);
            sircrit("critical message: %d", (n * j) + n);
            siralert("alert message: %d", (n * j) + n);
            siremerg("emergency message: %d", (n * j) + n);
        }
    }

    sir_cleanup();

    return 0;
}
