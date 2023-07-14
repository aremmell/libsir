#!/usr/bin/env zsh

setopt sh_word_split

SRC="dummy_plugin.c"
HDR="dummy_header.h"
INTDIR="/Users/ryan/Documents/Development/libsir/build/obj"
OUTOBJ="${INTDIR}/dummy_plugin.o"
OUTDIR="/Users/ryan/Documents/Development/libsir/build/lib"
OUTFILE="${OUTDIR}/dummy_plugin.so"

CFLAGS="-std=c11 -fPIC -O3 -DNDEBUG -I../../"
LDFLAGS="-shared"

clang -v -o "${OUTOBJ}" -MMD -c "${SRC}" ${CFLAGS} && \
clang -v -o "${OUTFILE}" "${OUTOBJ}" ${CFLAGS} "${LDFLAGS}" || echo "failed to build!"
