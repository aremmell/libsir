#!/usr/bin/env sh

printf 'Making libsir static library...\n'
make static || {
    printf 'Failed to compile; exiting!'
    exit 1
}

printf 'Ensuring build/bin exists...\n'
test -d build/bin || mkdir -p build/bin

printf 'Compiling C++ test rig...\n'
c++ --std=c++20 -c tests/tests.cc -o build/obj/tests.cc.o -Iinclude && \
c++ -o build/bin/sirtests++ build/obj/tests.cc.o -Lbuild/lib -lsir_s && \
printf 'Created build/bin/sirtests++\n'
