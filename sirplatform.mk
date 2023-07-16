#
# libsir -- sirplatform.mk
# https://github.com/aremmell/libsir
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman
#

# MinGW-w64 and standard Unix
ifneq "$(findstring mingw,$(CC))" ""
  MINGW?=1
endif
ifeq ($(MINGW),1)
  ifneq "$(findstring gcc,$(CC))" ""
    CFLAGS+=-Wno-unknown-pragmas
  endif
  PLATFORM_DLL_EXT=.dll
  PLATFORM_EXE_EXT=.exe
  PLATFORM_LIB_EXT=.lib
  PLATFORM_LIBS=-lshlwapi -lws2_32
else
  PLATFORM_DLL_EXT=.so
  PLATFORM_LIB_EXT=.a
endif

# Enable MinGW MSVCRT workaround?
ifeq ($(SIR_MSVCRT_MINGW),1)
  CFLAGS+=-DSIR_MSVCRT_MINGW
endif

# Oracle Studio C
ifneq "$(findstring suncc,$(CC))" ""
  SUNPRO?=1
endif
ifeq ($(SUNPRO),1)
  CFLAGS+=-fcommon
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
  MMDOPT=-xMMD
  PTHOPT=-mt=yes
else
  FORTIFY_FLAGS=-D_FORTIFY_SOURCE=2
  MMDOPT=-MMD
  PTHOPT=-pthread
endif

# Chibicc and Intel C++ Compiler Classic
ifneq "$(findstring icc,$(CC))" ""
  ifneq "$(findstring chibicc,$(CC))" ""
    FORTIFY_FLAGS=-U_FORTIFY_SOURCE
    PTHOPT=
  else
    INTELC?=1
  endif
endif
ifeq ($(INTELC),1)
  CFLAGS+=-diag-disable=188
  CFLAGS+=-diag-disable=191
  CFLAGS+=-diag-disable=10441
  CFLAGS+=-diag-disable=10148
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
endif

# NVIDIA HPC SDK C Compiler
ifneq "$(findstring nvc,$(CC))" ""
  NVIDIAC?=1
endif
ifeq ($(NVIDIAC),1)
  CFLAGS+=--diag_suppress mixed_enum_type
  CFLAGS+=--diag_suppress cast_to_qualified_type
  CFLAGS+=--diag_suppress nonstd_ellipsis_only_param
endif

# IBM XL C/C++ and GCC for AIX
ifneq "$(findstring AIX,$(shell uname -s 2> /dev/null))" ""
  ifneq "$(findstring xlc,$(CC))" ""
    IBMXLC?=1
  endif
endif
ifneq "$(findstring powerpc-ibm-aix7,$(CC))" ""
  AIXTLS?=1
endif
ifeq ($(IBMXLC),1)
  NO_DEFAULT_CFLAGS=1
  CFLAGS+=-O3 -I. -I.. -qtls -qthreaded -qinfo=mt -qformat=all -qpic=small
  SIR_SHARED=-qmkshrobj
  MMDOPT=
  PTHOPT=
endif
ifeq ($(AIXTLS),1)
  CFLAGS+=-ftls-model=initial-exec
endif

# Default way to link shared library?
SIR_SHARED?=-shared
