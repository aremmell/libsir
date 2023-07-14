#!/usr/bin/env bash

THISDIR="$(dirname "$(readlink -f "$0")")"
INTDIR="${THISDIR}/../../build/obj"
OUTOBJ="${INTDIR}/dummy_plugin.o"
OUTDIR="${THISDIR}/../../build/lib"
OUTFILE="${OUTDIR}/dummy_plugin.so"
SRC="${THISDIR}/dummy_plugin.c"
HDR="${THISDIR}/dummy_header.h"

CFLAGS="-std=c11 -fPIC -O3 -DNDEBUG -I. -I../../"
LDFLAGS="-shared"

cc -o "${OUTOBJ}" -MMD -c "${SRC}" ${CFLAGS} && \
cc -o "${OUTFILE}" "${OUTOBJ}" ${CFLAGS} ${LDFLAGS} && echo "built ${OUTFILE} successfully."
