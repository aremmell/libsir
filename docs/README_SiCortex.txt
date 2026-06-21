Are you a SiCortex supercomputer user who needs to build libsir?

You can, tested on SiCortex 4.0-FT with PathScale 4.0.10 and GCC 4.2.3.

First build sirinternal.o using GCC 4.2.3, and the rest of libsir using
PathScale 4.0.10.

Note that PathScale 3.3 or 4.0.8 are insufficient, and the C++ wrapper
is not built as it requires a C++20 compiler.

export NO_DEFAULT_CFLAGS=1
export SIR_CSTD=-std=gnu9x
export WFLAGS="-Wall -Wno-missing-braces"
export CFLAGS="-D__OPEN64__ -Iinclude $WFLAGS -fno-strict-aliasing -fPIC -O3"
export LDFLAGS="-fPIC -O3"
export LIBDL="-ldl -lrt"

env CC=gcc make LIBDL="$LIBDL" build/obj/src/sirinternal.o
env CC=pathcc make LIBDL="$LIBDL"

