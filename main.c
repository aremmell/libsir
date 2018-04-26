#include <stdio.h>
#include <stdlib.h>

#include "sir.h"

int main(int argc, char** argv) {

    sirinit si = {0};

    si.stdErrLevels = SIRL_ALL;    

    if (sir_init(&si)) {
        fprintf(stderr, "Failed to initialize SIR!\n");
        return 1;
    }

    // TODO: add file.

    sirdebug("debug message: %d", 123);

    sirinfo("longlier info message: %d", 123);

    sirnotice("longlier longlier notice message: %d", 123);
    
    sirwarn("longlier longlier longlier warning message: %d", 123);

    sirerror("longlier longlier error message: %d", 123);

    sircrit("longlier longlier longlier longlier longlier longlier longlier critical message: %d", 123);

    siralert("alert message: %d", 123);

    siremerg("emergency message: %d", 123);

    sir_cleanup();

    return 0;
}
