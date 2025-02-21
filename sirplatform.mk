# sirplatform.mk

################################################################################
#
# Version: 2.2.6
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
################################################################################

################################################################################
# Default optimizations

OPTFLAGS?=-O3

################################################################################
# What system?

UNAME_S:=$(shell uname -s 2> /dev/null)

################################################################################
# DUMA?

ifeq ($(DUMA),1)
  EXTRA_LIBS+=-l:libduma.a
  SIR_CFLAGS+=-DDUMA=1
  SIR_SHFLAGS=$(subst -l:libduma.a,,$(SIR_LDFLAGS))
endif

################################################################################
# OpenOSC?

ifeq ($(OPENOSC),1)
  EXTRA_LIBS+=-lopenosc
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
  SIR_CFLAGS+=-include openosc.h
  SIR_CSTD=-std=gnu11
  SIR_XSTD=-std=gnu++20
endif

################################################################################
# MinGW-w64 and standard Unix

ifneq "$(findstring mingw,$(CC))" ""
  MINGW?=1
endif

ifeq ($(MINGW),1)
  LIBDL=
  PLATFORM_DLL_EXT=.dll
  PLATFORM_EXE_EXT=.exe
  PLATFORM_LIB_EXT=.lib
  PLATFORM_LIBS=-lshlwapi -lws2_32
  ifneq "$(findstring gcc,$(CC))" ""
    SIR_CFLAGS+=-Wno-unknown-pragmas
  endif
else
  PLATFORM_DLL_EXT=.so
  PLATFORM_LIB_EXT=.a
endif

################################################################################
# Emscripten for Node.js

ifneq "$(findstring emcc,$(CC))" ""
  EMSCRIPTEN?=1
endif

ifeq ($(EMSCRIPTEN),1)
  AR=emar
  RANLIB=emranlib
  SIR_CFLAGS+=-pthread
  SIR_XFLAGS+=-pthread
  SIR_CSTD=-std=gnu11
  SIR_XSTD=-std=gnu++20
  SIR_LDFLAGS+=-lnoderawfs.js -lnodefs.js -s PROXY_TO_PTHREAD -s EXIT_RUNTIME
  SIR_SHARED=-Wno-emcc -shared
  SIR_NO_PLUGINS=1
endif

################################################################################
# LLVM-MinGW ARM64/ARMv7

ifneq "$(findstring armv7-w64-mingw32-clang,$(CC))" ""
  LLVM_RANLIB?=1
endif

ifneq "$(findstring aarch64-w64-mingw32-clang,$(CC))" ""
  LLVM_RANLIB?=1
endif

ifeq ($(LLVM_RANLIB),1)
  PLATFORM_LIB_EXT=.a
  RANLIB=llvm-ranlib
endif

################################################################################
# Haiku

ifneq "$(findstring Haiku,$(UNAME_S))" ""
  HAIKU?=1
endif

ifeq ($(HAIKU),1)
  LIBDL=
endif

################################################################################
# OpenBSD

ifneq "$(findstring OpenBSD,$(UNAME_S))" ""
  OPENBSD?=1
endif

ifeq ($(OPENBSD),1)
  LIBDL=
endif

################################################################################
# NetBSD

ifneq "$(findstring NetBSD,$(UNAME_S))" ""
  NETBSD?=1
endif

ifeq ($(NETBSD),1)
  LIBDL=
endif

################################################################################
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
  EXTRA_LIBS+=-lmtmalloc
  SIR_CFLAGS+=-DMTMALLOC
endif

################################################################################
# Enable MinGW MSVCRT workaround?

ifeq ($(SIR_MSVCRT_MINGW),1)
  SIR_CFLAGS+=-DSIR_MSVCRT_MINGW
endif

################################################################################
# Oracle Studio C

ifneq "$(findstring suncc,$(CC))" ""
  SUNPRO?=1
endif

ifeq ($(SUNPRO),1)
  FORTIFY_FLAGS?=-U_FORTIFY_SOURCE
  MMDOPT=-xMMD
  PTHOPT=-mt=yes
else
  ifneq "$(findstring circle,$(CXX))" ""
    FORTIFY_FLAGS?=-D_FORTIFY_SOURCE=2
  else
    FORTIFY_FLAGS?=-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2
  endif
  MMDOPT=-MMD
  PTHOPT=-pthread
endif

################################################################################
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
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
  SIR_CFLAGS+=-diag-disable=188
  SIR_CFLAGS+=-diag-disable=191
  SIR_CFLAGS+=-diag-disable=10148
  SIR_CFLAGS+=-diag-disable=10441
  SIR_LDFLAGS+=-diag-disable=10441
endif

################################################################################
# QNX C/C++ Compiler

ifneq "$(findstring qcc,$(CC))" ""
  QNX?=1
endif

ifneq "$(findstring q++,$(CXX))" ""
  QNX?=1
endif

ifeq ($(QNX),1)
  LIBDL=
  PTHOPT=
endif

################################################################################
# NVIDIA HPC SDK C Compiler

ifneq "$(findstring nvc,$(CC))" ""
  NVIDIAC?=1
endif

ifeq ($(NVIDIAC),1)
  DBGFLAGS=-g
  SIR_CFLAGS+=-noswitcherror
  SIR_CFLAGS+=--diag_suppress cast_to_qualified_type
  SIR_CFLAGS+=--diag_suppress mixed_enum_type
  SIR_CFLAGS+=--diag_suppress nonstd_ellipsis_only_param
endif

################################################################################
# Kefir

ifneq "$(findstring kefir,$(CC))" ""
  KEFIR?=1
endif

ifeq ($(KEFIR),1)
  FORTIFY_FLAGS="-U_FORTIFY_SOURCE"
  MMDOPT=
  NO_DEFAULT_CFLAGS=1
  PTHOPT=
  SIR_CSTD=
  SIR_GSTD=
  ifeq ($(SIR_DEBUG),1)
    SIR_CFLAGS+=-O0 -DDEBUG -Iinclude $(SIR_FPIC)
  else
    SIR_CFLAGS+=$(OPTFLAGS) -DNDEBUG -Iinclude $(SIR_FPIC)
  endif
endif

################################################################################
# IBM i (OS/400) PASE

ifneq "$(findstring OS400,$(UNAME_S))" ""
  IBMOS400?=1
endif

ifeq ($(IBMOS400),1)
  DBGFLAGS=-g
  OBJECT_MODE=64
  export OBJECT_MODE
  SIR_CFLAGS+=-D_THREAD_SAFE
  SIR_XFLAGS+=-D_THREAD_SAFE
  ifneq "$(findstring gcc,$(CC))" ""
    OS400GCC=1
    SIR_CFLAGS+=-fPIC -maix64 -Wl,-b64 -Wl,-brtl
  endif
  ifneq "$(findstring g++,$(CXX))" ""
    OS400GCC=1
    SIR_XFLAGS+=-fPIC -maix64 -Wl,-b64 -Wl,-brtl
  endif
  ifeq ($(OS400GCC),1)
    DBGFLAGS=-Og
    WPEDANTIC=
    SIR_LDFLAGS+=-fPIC -maix64 -Wl,-b64 -Wl,-brtl
  endif
endif

################################################################################
# IBM XL C/C++ and GCC for AIX

ifneq "$(findstring AIX,$(UNAME_S))" ""
  IBMAIX?=1
  ifneq "$(findstring xlc,$(CC))" ""
    IBMXLC?=1
  endif
  ifneq "$(findstring clang++,$(CXX))" ""
    AIXCLANGXX?=1
  endif
endif

ifneq "$(findstring powerpc-ibm-aix7,$(CC))" ""
  AIXTLS?=1
endif

ifeq ($(IBMAIX),1)
  DBGFLAGS=-g
  OBJECT_MODE=64
  export OBJECT_MODE
  SIR_CFLAGS+=-D_THREAD_SAFE
  SIR_XFLAGS+=-D_THREAD_SAFE
  ifneq "$(findstring gcc,$(CC))" ""
    IBMAIX_GCC=1
    SIR_CFLAGS+=-fPIC -maix64 -ftls-model=global-dynamic
  endif
  ifneq "$(findstring gcc,$(CXX))" ""
    IBMAIX_GCC=1
    SIR_XFLAGS+=-fPIC -maix64 -ftls-model=global-dynamic
  endif
  ifeq ($(IBMAIX_GCC),1)
    SIR_LDFLAGS+=-fPIC -maix64 -ftls-model=global-dynamic
  endif
endif

ifeq ($(IBMXLC),1)
  AIX_STDFLAGS=-Iinclude -qtls=global-dynamic -qthreaded -qformat=all:nonlt -qpic
  MMDOPT=
  NO_DEFAULT_CFLAGS=1
  PTHOPT=
  SIR_CSTD=
  SIR_SHARED=-qmkshrobj
  ifeq ($(SIR_DEBUG),1)
    SIR_CFLAGS+=$(DBGFLAGS) -O0 -DDEBUG $(AIX_STDFLAGS)
    SIR_XFLAGS+=$(DBGFLAGS) -O0 -DDEBUG $(AIX_STDFLAGS)
  else
    SIR_CFLAGS+=$(OPTFLAGS) -DNDEBUG $(AIX_STDFLAGS)
    SIR_XFLAGS+=$(OPTFLAGS) -DNDEBUG $(AIX_STDFLAGS)
  endif
endif

ifeq ($(AIXCLANGXX),1)
  SIR_CFLAGS+=-D_LIBCPP_NO_ABI_TAG
endif

ifeq ($(AIXTLS),1)
  DBGFLAGS=-g
  SIR_CFLAGS+=-fPIC -ftls-model=global-dynamic
  SIR_LDFLAGS+=-fPIC -ftls-model=global-dynamic
  SIR_XFLAGS+=-fPIC -ftls-model=global-dynamic
endif

################################################################################
# CompCert-C - https://www.absint.com/compcert - https://compcert.org
# https://gricad-gitlab.univ-grenoble-alpes.fr/certicompil/Chamois-CompCert
# Tested with Kalray/Verimag Chamois CompCert as of 2023-11-30

ifneq "$(findstring compcert,$(CC))" ""
  COMPCERT?=1
endif

ifeq ($(COMPCERT),1)
  MMDOPT=
  PTHOPT=
  WPEDANTIC=
  SIR_FPIC=
  FORTIFY_FLAGS=-U_FORTIFY_SOURCE
  WARNEXTRA=-Wno-zero-length-array -Wno-c11-extensions
  SIR_NO_SHARED=1
endif

################################################################################
# Circle (C++ only)

ifneq "$(findstring circle,$(CXX))" ""
  CIRCLECPP?=1
endif

ifeq ($(CIRCLECPP),1)
  CXXFLAGS+=-DSIR_NO_STD_IOSTREAM=1
  WPEDANTIC=
endif

################################################################################
# Enable GCC's `-fweb` optimization

ifneq ($(SIR_DEBUG),1)
  ifneq "$(findstring gcc,$(CC))" ""
    SIR_CFLAGS+=-fweb
    SIR_CXFLAGS+=-fweb
  endif
endif

################################################################################
# Default flags for linking a shared library?

SIR_SHARED?=-shared

################################################################################
