#
# libsir -- Makefile
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
PREFIX     ?= /usr/local
INSTALLLIB  = $(DESTDIR)$(PREFIX)/lib
INSTALLINC  = $(DESTDIR)$(PREFIX)/include
INSTALLSH   = ./build-aux/install-sh
RANLIB     ?= ranlib
LDCONFIG   ?= ldconfig
SHELL      := $(shell env sh -c 'PATH="$$(command -p getconf PATH)" command -v sh')

# platform specifics
include sirplatform.mk

# base CFLAGS
ifndef NO_DEFAULT_CFLAGS
  CFLAGS += -Wall -Wextra -Wpedantic -std=c11 -I. -fPIC
endif

# debug/non-debug CFLAGS
ifeq ($(SIR_DEBUG),1)
  CFLAGS += -g -O0 -DDEBUG -U_FORTIFY_SOURCE
else
  CFLAGS += -O3 -DNDEBUG $(FORTIFY_FLAGS)
endif

# enable internal diagnostic logging?
ifeq ($(SIR_SELFLOG),1)
  CFLAGS += -DSIR_SELFLOG
endif

# enable assertions?
ifeq ($(SIR_ASSERT_ENABLED),1)
  CFLAGS += -DSIR_ASSERT_ENABLED
endif

# disable system loggers?
ifeq ($(SIR_NO_SYSTEM_LOGGERS),1)
  CFLAGS += -DSIR_NO_SYSTEM_LOGGERS
endif

# dependencies
LIBS = $(PTHOPT)

# for test rig and example:
# link with static library, not shared
LDFLAGS += $(LIBS) -L$(LIBDIR) -lsir_s $(PLATFORM_LIBS)

# translation units
TUS := $(wildcard *.c)

# intermediate files
_OBJ = $(patsubst %.c, %.o, $(TUS))
OBJ  = $(patsubst %, $(INTDIR)/%, $(_OBJ))

# shared library
OBJ_SHARED     = $(patsubst %.o, $(INTDIR)/%.o, $(_OBJ))
OUT_SHARED_FN  = libsir$(PLATFORM_DLL_EXT)
OUT_SHARED     = $(LIBDIR)/$(OUT_SHARED_FN)
LDFLAGS_SHARED = $(LIBS) $(PLATFORM_LIBS)

# static library
OBJ_STATIC     = $(OBJ_SHARED)
OUT_STATIC_FN  = libsir_s$(PLATFORM_LIB_EXT)
OUT_STATIC     = $(LIBDIR)/$(OUT_STATIC_FN)

# console example
OBJ_EXAMPLE    = $(INTDIR)/$(EXAMPLE)/$(EXAMPLE).o
OUT_EXAMPLE    = $(BINDIR)/sirexample$(PLATFORM_EXE_EXT)

# console test rig
OBJ_TESTS      = $(INTDIR)/$(TESTS)/$(TESTS).o
OUT_TESTS      = $(BINDIR)/sirtests$(PLATFORM_EXE_EXT)

# ##########
# targets
# ##########

.DEFAULT_GOAL := all
.PHONY: all
all: $(OUT_SHARED) $(OUT_STATIC) $(OUT_EXAMPLE) $(OUT_TESTS)

-include $(INTDIR)/*.d

$(OBJ_EXAMPLE): $(EXAMPLE)/$(EXAMPLE).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) -I..

$(OBJ_TESTS): $(TESTS)/$(TESTS).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) -I..

$(INTDIR)/%.o: %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS)

.PHONY: shared
shared: $(OUT_SHARED)
$(OUT_SHARED): $(OBJ_SHARED)
	@mkdir -p $(@D)
	$(CC) -shared -o $(OUT_SHARED) $^ $(CFLAGS) $(LDFLAGS_SHARED)
	-@echo built $(OUT_SHARED) successfully.

.PHONY: static
static: $(OUT_STATIC)
$(OUT_STATIC): $(OUT_SHARED)
	@mkdir -p $(@D)
	ar -cr $(OUT_STATIC) $(OBJ_SHARED)
	-@($(RANLIB) "$(OUT_STATIC)" || true) > /dev/null 2>&1
	-@echo built $(OUT_STATIC) successfully.

.PHONY: example
example: $(OUT_EXAMPLE)
$(OUT_EXAMPLE): $(OUT_STATIC) $(OBJ_EXAMPLE)
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	$(CC) -o $(OUT_EXAMPLE) $(OBJ_EXAMPLE) $(CFLAGS) -I.. $(LDFLAGS)
	-@echo built $(OUT_EXAMPLE) successfully.

$(BINDIR)/file.exists:
	@mkdir -p $(BINDIR)
	@touch $(BINDIR)/file.exists > /dev/null

.PHONY: tests
tests: $(OUT_TESTS)
$(OUT_TESTS): $(OUT_STATIC) $(OBJ_TESTS) $(BINDIR)/file.exists
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	$(CC) -o $(OUT_TESTS) $(OBJ_TESTS) $(CFLAGS) -I.. $(LDFLAGS)
	-@echo built $(OUT_TESTS) successfully.

.PHONY: docs
docs: $(OUT_STATIC)
	@doxygen Doxyfile
	-@find docs -name '*.png' -exec advpng -z4 "{}" \
		2> /dev/null \; 2> /dev/null || true
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
	-($(RANLIB) "$(INSTALLLIB)/$(OUT_STATIC_FN)" || true) > /dev/null 2>&1
	$(INSTALLSH) -C -m 644 "sir.h" "$(INSTALLINC)"
	-@echo installed libsir successfully.

.PHONY: clean distclean
clean distclean:
	@rm -rf $(BUILDDIR) > /dev/null 2>&1
	@rm -rf ./*.log > /dev/null 2>&1
	@rm -rf ./*.d > /dev/null 2>&1
	-@echo build directory and log files cleaned successfully.

.PHONY: printvars printenv
printvars printenv:
	-@printf '%s: ' "FEATURES"; printf '%s ' "$(.FEATURES)"; printf '%s\n' ""
	-@$(foreach V,$(sort $(.VARIABLES)), \
		$(if $(filter-out environment% default automatic,$(origin $V)), \
			$(if $(strip $($V)),$(info $V: [$($V)]),)))
	-@true > /dev/null 2>&1

.PHONY: print-%
print-%:
	-@$(info $*: [$($*)] ($(flavor $*). set by $(origin $*)))@true
	-@true > /dev/null 2>&1
