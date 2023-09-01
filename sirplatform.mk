# sirplatform.mk
# libsir: https://github.com/aremmell/libsir
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

##############################################################################
# What system?

UNAME_S:=$(shell uname -s 2> /dev/null)

##############################################################################
# DUMA?

ifdef DUMA
  EXTRA_LIBS+=-l:libduma.a
  SIR_CFLAGS+=-DDUMA=1
  SIR_SHFLAGS=$(subst -l:libduma.a,,$(SIR_LDFLAGS))
endif

##############################################################################
# MinGW-w64 and standard Unix

ifneq "$(findstring mingw,$(CC))" ""
  MINGW?=1
endif

ifeq ($(MINGW),1)
  ifneq "$(findstring gcc,$(CC))" ""
    SIR_CFLAGS+=-Wno-unknown-pragmas
  endif
  LIBDL=
  PLATFORM_DLL_EXT=.dll
  PLATFORM_EXE_EXT=.exe
  PLATFORM_LIB_EXT=.lib
  PLATFORM_LIBS=-lshlwapi -lws2_32
else
  PLATFORM_DLL_EXT=.so
  PLATFORM_LIB_EXT=.a
endif

##############################################################################
# LLVM-MinGW ARMv7

ifneq "$(findstring armv7-w64-mingw32-clang,$(CC))" ""
  RANLIB=llvm-ranlib
  PLATFORM_LIB_EXT=.a
endif

##############################################################################
# LLVM-MinGW ARM64

ifneq "$(findstring aarch64-w64-mingw32-clang,$(CC))" ""
  RANLIB=llvm-ranlib
  PLATFORM_LIB_EXT=.a
endif

##############################################################################
# Haiku

ifneq "$(findstring Haiku,$(UNAME_S))" ""
  HAIKU?=1
endif

ifeq ($(HAIKU),1)
  LIBDL=
endif

##############################################################################
# OpenBSD

ifneq "$(findstring OpenBSD,$(UNAME_S))" ""
  OPENBSD?=1
endif

ifeq ($(OPENBSD),1)
  LIBDL=
endif

##############################################################################
# NetBSD

ifneq "$(findstring NetBSD,$(UNAME_S))" ""
  NETBSD?=1
endif

ifeq ($(NETBSD),1)
  LIBDL=
endif

##############################################################################
# SunOS

ifneq "$(findstring SunOS,$(UNAME_S))" ""
  UNAME_O:=$(shell uname -o 2> /dev/null)
  ifneq "$(findstring Solaris,$(UNAME_O))" ""
    SUNSYSV?=1
  endif
  ifneq "$(findstring illumos,$(UNAME_O))" ""
    SUNSYSV?=1
  endif
endif

ifeq ($(SUNSYSV),1)
  SIR_CFLAGS+=-DMTMALLOC
  EXTRA_LIBS+=-lmtmalloc
endif

##############################################################################
# Enable MinGW MSVCRT workaround?

ifeq ($(SIR_MSVCRT_MINGW),1)
  SIR_CFLAGS+=-DSIR_MSVCRT_MINGW
endif

##############################################################################
# Oracle Studio C

ifneq "$(findstring suncc,$(CC))" ""
  SUNPRO?=1
endif

ifeq ($(SUNPRO),1)
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
  MMDOPT=-xMMD
  PTHOPT=-mt=yes
else
  FORTIFY_FLAGS=-D_FORTIFY_SOURCE=2
  MMDOPT=-MMD
  PTHOPT=-pthread
endif

##############################################################################
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
  SIR_CFLAGS+=-diag-disable=188
  SIR_CFLAGS+=-diag-disable=191
  SIR_CFLAGS+=-diag-disable=10441
  SIR_CFLAGS+=-diag-disable=10148
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
endif

##############################################################################
# NVIDIA HPC SDK C Compiler

ifneq "$(findstring nvc,$(CC))" ""
  NVIDIAC?=1
endif

ifeq ($(NVIDIAC),1)
  DBGFLAGS=-g
  SIR_CFLAGS+=--diag_suppress mixed_enum_type
  SIR_CFLAGS+=--diag_suppress cast_to_qualified_type
  SIR_CFLAGS+=--diag_suppress nonstd_ellipsis_only_param
endif

##############################################################################
# IBM XL C/C++ and GCC for AIX

ifneq "$(findstring AIX,$(UNAME_S))" ""
  IBMAIX?=1
  ifneq "$(findstring xlc,$(CC))" ""
    IBMXLC?=1
  endif
endif

ifneq "$(findstring powerpc-ibm-aix7,$(CC))" ""
  AIXTLS?=1
endif

ifeq ($(IBMAIX),1)
  DBGFLAGS=-g
endif

ifeq ($(IBMXLC),1)
  NO_DEFAULT_CFLAGS=1
  ifeq ($(SIR_DEBUG),1)
    CFLAGS+=$(DBGFLAGS) -O0 -DDEBUG -Iinclude -qtls -qthreaded -qinfo=mt -qformat=all
  else
    CFLAGS+=-DNDEBUG $(OPTFLAGS) -Iinclude -qtls -qthreaded -qinfo=mt -qformat=all -qpic=small
  endif
  SIR_SHARED=-qmkshrobj
  MMDOPT=
  PTHOPT=
endif

ifeq ($(AIXTLS),1)
  DBGFLAGS=-g
  SIR_CFLAGS+=-ftls-model=initial-exec
endif

##############################################################################
# Default flags for linking a shared library?

SIR_SHARED?=-shared

##############################################################################
