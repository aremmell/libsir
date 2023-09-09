#!/usr/bin/env sh

selflog=true
CFLAGS="--std=c++20 -Wall -Wextra -Wpedantic -O3"

test ${selflog} = true && {
    make_env="env SIR_SELFLOG=1"
    CFLAGS="${CFLAGS} -DSIR_SELFLOG"
}

printf 'Making libsir static library...\n'
${make_env} make static || {
    printf 'Failed to compile; exiting!'
    exit 1
}

printf 'Ensuring build/bin exists...\n'
test -d build/bin || mkdir -p build/bin

printf 'Compiling C++ test rig...\n'

c++ -c tests/tests.cc -o build/obj/tests.cc.o ${CFLAGS} -Iinclude && \
c++ -o build/bin/sirtests++ build/obj/tests.cc.o -Lbuild/lib -lsir_s && \
printf 'Created build/bin/sirtests++\n'
