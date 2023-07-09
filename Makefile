#
# libsir
# https://github.com/aremmell/libsir
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman
#

BUILDDIR    = ./build
DOCSDIR     = docs
TESTS       = tests
EXAMPLE     = example
INTDIR      = $(BUILDDIR)/obj
LIBDIR      = $(BUILDDIR)/lib
BINDIR      = $(BUILDDIR)/bin
PREFIX     ?= $(DESTDIR)/usr/local
INSTALLLIB  = $(PREFIX)/lib
INSTALLINC  = $(PREFIX)/include
INSTALLSH   = build-aux/install-sh
RANLIB     ?= ranlib
LDCONFIG   ?= ldconfig
XSHELL      = $(shell env PATH="$$(command -p getconf PATH)" command -v sh)
SHELL      := $(XSHELL)

# base CFLAGS
CFLAGS += -Wall -Wextra -Wpedantic -std=c11 -I. -fPIC

# MinGW compiler-specific flags
ifneq "$(findstring mingw,$(CC))" ""
	MINGW ?= 1
endif
ifeq ($(MINGW),1)
  ifneq "$(findstring gcc,$(CC))" ""
	CFLAGS += -Wno-unknown-pragmas
  endif
	MINGW_LIBS=-lshlwapi -lws2_32
endif

# Oracle compiler-specific flags
ifneq "$(findstring suncc,$(CC))" ""
	SUNPRO ?= 1
endif
ifeq ($(SUNPRO),1)
	CFLAGS        += -fcommon
	FORTIFY_FLAGS ?= -U_FORTIFY_SOURCE
	MMDOPT        ?= -xMMD
	PTHOPT        ?= -mt=yes
else
	FORTIFY_FLAGS ?= -D_FORTIFY_SOURCE=2
	MMDOPT        ?= -MMD
	PTHOPT        ?= -pthread
endif

# MinGW MSVCRT workaround
ifeq ($(SIR_MSVCRT_MINGW),1)
	CFLAGS += -DSIR_MSVCRT_MINGW
endif

# debug/non-debug CFLAGS
ifeq ($(SIR_DEBUG),1)
	CFLAGS += -g -O0 -DDEBUG -U_FORTIFY_SOURCE
else
	CFLAGS += -O3 -DNDEBUG $(FORTIFY_FLAGS)
endif

# enable internal diagnostic logging
ifeq ($(SIR_SELFLOG),1)
	CFLAGS += -DSIR_SELFLOG
endif

ifeq ($(SIR_ASSERT_ENABLED),1)
	CFLAGS += -DSIR_ASSERT_ENABLED
endif

# on Windows, automatically defined by the preprocessor.
ifeq ($(SIR_NO_SYSTEM_LOGGERS),1)
	CFLAGS += -DSIR_NO_SYSTEM_LOGGERS
endif

# dependencies
LIBS = $(PTHOPT)

# for test rig and example:
# link with static library, not shared
LDFLAGS += $(LIBS) -L$(LIBDIR) -lsir_s $(MINGW_LIBS)

# translation units
TUS := $(wildcard *.c)

# intermediate files
_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTDIR)/%, $(_OBJ))

# shared library
OBJ_SHARED     = $(patsubst %.o, $(INTDIR)/%.o, $(_OBJ))
OUT_SHARED_NM  = libsir.so
OUT_SHARED     = $(LIBDIR)/$(OUT_SHARED_NM)
LDFLAGS_SHARED = $(LIBS) $(MINGW_LIBS)

# static library
OBJ_STATIC     = $(OBJ_SHARED)
OUT_STATIC_NM  = libsir_s.a
OUT_STATIC     = $(LIBDIR)/$(OUT_STATIC_NM)

# console example
OBJ_EXAMPLE    = $(INTDIR)/$(EXAMPLE)/$(EXAMPLE).o
OUT_EXAMPLE    = $(BINDIR)/sirexample

# console test rig
OBJ_TESTS      = $(INTDIR)/$(TESTS)/$(TESTS).o
OUT_TESTS      = $(BINDIR)/sirtests

# ##########
# targets
# ##########

all: prep shared static example tests

-include $(INTDIR)/*.d

$(BUILDDIR)   : prep
$(INTDIR)     : $(BUILDDIR)
$(LIBDIR)     : $(BUILDDIR)
$(BINDIR)     : $(BUILDDIR)
$(OBJ_SHARED) : $(INTDIR)
$(OBJ_TESTS)  : $(OBJ_SHARED)
$(OBJ_EXAMPLE): $(OBJ_SHARED)

$(OBJ_EXAMPLE): $(EXAMPLE)/$(EXAMPLE).c $(DEPS)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) -I..

$(OBJ_TESTS): $(TESTS)/$(TESTS).c $(DEPS)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) -I..

$(INTDIR)/%.o: %.c $(DEPS)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS)

prep:
	$(shell mkdir -p $(BUILDDIR) && \
			mkdir -p $(INTDIR)/$(EXAMPLE) && \
			mkdir -p $(INTDIR)/$(TESTS) && \
			mkdir -p $(LIBDIR) && \
	        mkdir -p $(BINDIR))
	-@echo directories prepared successfully.

shared: $(OBJ_SHARED)
	$(CC) -shared -o $(OUT_SHARED) $^ $(CFLAGS) $(LDFLAGS_SHARED)
	-@echo built $(OUT_SHARED) successfully.

static: shared
	ar -cr $(OUT_STATIC) $(OBJ_SHARED)
	-($(RANLIB) "$(OUT_STATIC)" || true) > /dev/null 2>&1
	-@echo built $(OUT_STATIC) successfully.

example: static $(OBJ_EXAMPLE)
	$(CC) -o $(OUT_EXAMPLE) $(OBJ_EXAMPLE) $(CFLAGS) -I.. $(LDFLAGS)
	-@echo built $(OUT_EXAMPLE) successfully.

tests: static $(OBJ_TESTS)
	$(CC) -o $(OUT_TESTS) $(OBJ_TESTS) $(CFLAGS) -I.. $(LDFLAGS)
	$(shell touch $(BINDIR)/file.exists)
	-@echo built $(OUT_TESTS) successfully.

docs: static
	@doxygen Doxyfile
	-@echo built documentation successfully.

.PHONY: install
install: $(INSTALLSH)
	@test -x $(INSTALLSH) || \
		{ printf 'Error: %s not executable.\n' "$(INSTALLSH)"; exit 1; }
	+@test -f "$(OUT_STATIC)" || $(MAKE) static
	+@test -f "$(OUT_SHARED)" || $(MAKE) shared
	-@echo installing libraries to $(INSTALLLIB) and headers to $(INSTALLINC)...
	$(INSTALLSH) -C -m 755 "$(OUT_SHARED)" "$(INSTALLLIB)"
	-($(LDCONFIG) || true) > /dev/null 2>&1
	$(INSTALLSH) -C -m 644 "$(OUT_STATIC)" "$(INSTALLLIB)"
	-($(RANLIB) "$(INSTALLLIB)/$(OUT_STATIC_NM)" || true) > /dev/null 2>&1
	$(INSTALLSH) -C -m 644 "sir.h" "$(INSTALLINC)"
	-@echo installed libsir successfully.

.PHONY: clean distclean
clean distclean:
	$(shell rm -rf "$(BUILDDIR)/" > /dev/null 2>&1 ; \
			rm -f ./*.log > /dev/null 2>&1)
	-@echo build directory and log files cleaned successfully.

.PHONY: printvars printenv
printvars printenv:
	-@$(foreach V,$(sort $(.VARIABLES)), \
		$(if $(filter-out environment% default automatic,$(origin $V)), \
		$(if $(strip $($V)),$(info $V: [$($V)]),)))
	-@true > /dev/null 2>&1

.PHONY: print-%
print-%:
	-@$(info $*: [$($*)] ($(flavor $*). set by $(origin $*)))@true
	-@true > /dev/null 2>&1
