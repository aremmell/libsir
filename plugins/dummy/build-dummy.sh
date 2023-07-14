#!/usr/bin/env bash

BASEDIR="${HOME}/Documents/Development/libsir"
INTDIR="${BASEDIR}/build/obj"
OUTOBJ="${INTDIR}/dummy_plugin.o"
OUTDIR="${BASEDIR}/build/lib"
OUTFILE="${OUTDIR}/dummy_plugin.so"
THISDIR="${BASEDIR}/plugins/dummy"
SRC="${THISDIR}/dummy_plugin.c"
HDR="${THISDIR}/dummy_header.h"

CFLAGS="-std=c11 -fPIC -O3 -DNDEBUG -I. -I../../"
LDFLAGS="-shared"

cc -o "${OUTOBJ}" -MMD -c "${SRC}" ${CFLAGS} && \
cc -o "${OUTFILE}" "${OUTOBJ}" ${CFLAGS} ${LDFLAGS}
