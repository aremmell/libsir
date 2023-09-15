# Makefile
# libsir: https://github.com/aremmell/libsir
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

##############################################################################
# Defaults

.NOTPARALLEL:
SHELL       := $(shell env sh -c 'PATH="$$(command -p getconf PATH)" command -v sh')
BUILDDIR     = ./build
LOGDIR       = ./logs
LINTSH       = ./.lint.sh
DOCSDIR      = docs
TESTS_SHX    = tests_shared
TESTS        = tests
TESTSXX      = tests++
EXAMPLE      = example
UTILS        = utils
MCMB         = mcmb
INTDIR       = $(BUILDDIR)/obj
LIBDIR       = $(BUILDDIR)/lib
BINDIR       = $(BUILDDIR)/bin
BINDINGSDIR  = bindings
PREFIX      ?= /usr/local
INSTALLLIB   = $(DESTDIR)$(PREFIX)/lib
INSTALLINC   = $(DESTDIR)$(PREFIX)/include
INSTALLSH    = ./build-aux/install-sh
RANLIB      ?= ranlib
LDCONFIG    ?= ldconfig
PLUGINS      = ./plugins
PLUGINNAMES  = $(subst $(PLUGINS)/,,$(wildcard $(PLUGINS)/*))
PLUGPREFIX   = plugin_
SIR_FPIC    ?= -fPIC
AR          ?= ar
AR_CR       ?= $(AR) -cr
WPEDANTIC   ?= -Wpedantic

##############################################################################
# Flags

SIR_CFLAGS  := $(CFLAGS)
SIR_LDFLAGS := $(LDFLAGS)
SIR_SHFLAGS  = $(subst -static,,$(SIR_LDFLAGS))

##############################################################################
# Optimizations

OPTFLAGS ?= -O3

##############################################################################
# Platform specifics

-include sirplatform.mk

##############################################################################
# C standard

SIR_CSTD ?= -std=c11
SIR_GSTD ?= -std=gnu11
SIR_XSTD ?= -std=c++20

##############################################################################
# Base CFLAGS

ifneq ($(NO_DEFAULT_CFLAGS),1)
  SIR_CFLAGS += -Wall -Wextra $(strip $(strip $(WPEDANTIC) -Iinclude) $(SIR_FPIC))
endif

##############################################################################
# DEBUG/non-DEBUG CFLAGS

ifeq ($(SIR_DEBUG),1)
  DBGFLAGS ?= -g3
  ifneq ($(NO_DEFAULT_CFLAGS),1)
    SIR_CFLAGS += -O0 -DDEBUG -D_DEBUG $(DBGFLAGS) -U_FORTIFY_SOURCE
  endif
else
  ifneq ($(NO_DEFAULT_CFLAGS),1)
    SIR_CFLAGS += $(OPTFLAGS) -DNDEBUG $(FORTIFY_FLAGS)
  endif
endif

##############################################################################
# Enable internal diagnostic logging?

ifeq ($(SIR_SELFLOG),1)
  SIR_CFLAGS += -DSIR_SELFLOG
endif

##############################################################################
# Enable assertions?

ifeq ($(SIR_ASSERT_ENABLED),1)
  SIR_CFLAGS += -DSIR_ASSERT_ENABLED
endif

##############################################################################
# Disable system loggers?

ifeq ($(SIR_NO_SYSTEM_LOGGERS),1)
  SIR_CFLAGS += -DSIR_NO_SYSTEM_LOGGERS
endif

##############################################################################
# Disable plugins?

ifeq ($(SIR_NO_PLUGINS),1)
  SIR_CFLAGS += -DSIR_NO_PLUGINS
else
  LIBDL  ?= -ldl
  PGOALS  = plugins
endif

#############################################################################
# Developer profiling build?

ifeq ($(SIR_PERF_PROFILE),1)
  ifneq ($(SIR_NO_PLUGINS),1)
    $(warning Warning: SIR_PERF_PROFILE set without SIR_NO_PLUGINS.)
  endif
  ifneq ($(SIR_NO_SYSTEM_LOGGERS),1)
    $(warning Warning: SIR_PERF_PROFILE set without SIR_NO_SYSTEM_LOGGERS.)
  endif
  ifeq ($(SIR_SELFLOG),1)
    $(error Error: Both SIR_SELFLOG and SIR_PERF_PROFILE are set..)
  endif
  ifeq ($(SIR_DEBUG),1)
    $(error Error: Both SIR_DEBUG and SIR_PERF_PROFILE are set..)
  endif
  ifeq ($(DUMA),1)
    $(error Error: Both DUMA and SIR_PERF_PROFILE are set..)
  endif
  SIR_CFLAGS += -DSIR_PERF_PROFILE
endif

##############################################################################
# Libraries

LIBS = $(PTHOPT)

##############################################################################
# Linker flags

SIR_LDFLAGS += $(strip $(LIBS) -L)$(LIBDIR) $(strip $(strip $(PLATFORM_LIBS) $(LIBDL)) $(EXTRA_LIBS))

##############################################################################
# Static libsir for test rig and example

LIBSIR_S = -lsir_s

##############################################################################
# Translation units

TUS := $(wildcard src/*.c)

##############################################################################
# Intermediate files

_OBJ = $(strip $(patsubst %.c, %.o, $(TUS)))
OBJ  = $(strip $(patsubst %, $(INTDIR)/%, $(_OBJ)))

##############################################################################
# Shared library

OBJ_SHARED     = $(strip $(patsubst %.o, $(INTDIR)/%.o, $(_OBJ)))
OUT_SHARED_FN  = libsir$(PLATFORM_DLL_EXT)
OUT_SHARED     = $(LIBDIR)/$(OUT_SHARED_FN)

##############################################################################
# Static library

OBJ_STATIC     = $(OBJ_SHARED)
OUT_STATIC_FN  = libsir_s$(PLATFORM_LIB_EXT)
OUT_STATIC     = $(LIBDIR)/$(OUT_STATIC_FN)

##############################################################################
# Console example

OBJ_EXAMPLE    = $(INTDIR)/$(EXAMPLE)/$(EXAMPLE).o
OUT_EXAMPLE    = $(BINDIR)/sirexample$(PLATFORM_EXE_EXT)

##############################################################################
# Console test rig shared functionality

OBJ_TESTS_SHX   = $(INTDIR)/$(TESTS)/$(TESTS_SHX).o

##############################################################################
# Console test rig

OBJ_TESTS      = $(INTDIR)/$(TESTS)/$(TESTS).o
OUT_TESTS      = $(BINDIR)/sirtests$(PLATFORM_EXE_EXT)

##############################################################################
# Console C++ test rig

OBJ_TESTSXX    = $(INTDIR)/$(TESTS)/$(TESTSXX).o
OUT_TESTSXX    = $(BINDIR)/sirtests++$(PLATFORM_EXE_EXT)

##############################################################################
# Miniature combinatorics utility

OBJ_MCMB       = $(INTDIR)/$(MCMB)/$(MCMB).o
OUT_MCMB       = $(BINDIR)/mcmb$(PLATFORM_EXE_EXT)

##############################################################################
# Default goal

.DEFAULT_GOAL := all

.PHONY: all

all: $(PGOALS) $(OUT_SHARED) $(OUT_STATIC) $(OUT_EXAMPLE) $(OUT_TESTS)

##############################################################################
# Automatic dependencies

ifneq (,$(wildcard $(INTDIR)/*/*.d))
  -include $(wildcard $(INTDIR)/*/*.d)
endif

##############################################################################
# Compile example

$(OBJ_EXAMPLE): $(EXAMPLE)/$(EXAMPLE).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) $(SIR_CSTD) $(SIR_CFLAGS) -Iinclude -c -o $@ $<

##############################################################################
# Compile mcmb

$(OBJ_MCMB): $(UTILS)/$(MCMB)/$(MCMB).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) $(SIR_GSTD) $(SIR_CFLAGS) -c -o $@ $<

##############################################################################
# Compile functionality shared between C and C++ test rigs

$(OBJ_TESTS_SHX): $(TESTS)/$(TESTS_SHX).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) $(SIR_CSTD) $(SIR_CFLAGS) -Iinclude -c -o $@ $<

##############################################################################
# Compile tests

$(OBJ_TESTS): $(TESTS)/$(TESTS).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) $(SIR_CSTD) $(SIR_CFLAGS) -Iinclude -c -o $@ $<

##############################################################################
# Compile C sources

$(INTDIR)/%.o: %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) $(SIR_CSTD) $(SIR_CFLAGS) -c -o $@ $<

##############################################################################
# Compile tests++

SIR_XFLAGS  := $(SIR_CFLAGS) $(CXXFLAGS)

$(OBJ_TESTSXX): $(TESTS)/$(TESTSXX).cc $(DEPS)
	@mkdir -p $(@D)
	$(CXX) $(MMDOPT) $(SIR_XSTD) $(SIR_XFLAGS) -Iinclude -c -o $@ $<

##############################################################################
# Link shared library

.PHONY: shared

shared: $(OUT_SHARED)

$(OUT_SHARED): $(OBJ_SHARED)
	@mkdir -p $(@D)
	$(CC) $(SIR_SHARED) -o $(OUT_SHARED) $^ $(SIR_SHFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_SHARED)" 2> /dev/null

##############################################################################
# Link static library

.PHONY: static

static: $(OUT_STATIC)

$(OUT_STATIC): $(OUT_SHARED)
	@mkdir -p $(@D)
	$(AR_CR) $(OUT_STATIC) $(OBJ_SHARED)
	-@($(RANLIB) "$(OUT_STATIC)" || true) > /dev/null 2>&1
	-@printf 'built %s successfully.\n' "$(OUT_STATIC)" 2> /dev/null

##############################################################################
# Link example

.PHONY: example

example: $(OUT_EXAMPLE)

$(OUT_EXAMPLE): $(OUT_STATIC) $(OBJ_EXAMPLE)
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	$(CC) -o $(OUT_EXAMPLE) $(OBJ_EXAMPLE) -Iinclude $(LIBSIR_S) $(SIR_LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_EXAMPLE)" 2> /dev/null

##############################################################################
# Link miniature combinatorics utility

.PHONY: mcmb cmb

mcmb cmb: $(OUT_MCMB)

$(OUT_MCMB): $(OBJ_MCMB)
	mkdir -p $(@D)
	mkdir -p $(BINDIR)
	$(CC) -o $(OUT_MCMB) $(OBJ_MCMB) $(SIR_LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_MCMB)" 2> /dev/null

##############################################################################
# Link tests++

.PHONY: tests++ test++

tests++ test++: $(OUT_TESTSXX)

$(OUT_TESTSXX): $(OUT_STATIC) $(OBJ_TESTS_SHX) $(OBJ_TESTSXX)
	$(MAKE) --no-print-directory plugins
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	@mkdir -p $(LOGDIR)
	@touch $(BINDIR)/file.exists > /dev/null
	$(CXX) -o $(OUT_TESTSXX) $(OBJ_TESTS_SHX) $(OBJ_TESTSXX) -Iinclude $(LIBSIR_S) $(SIR_LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_TESTSXX)" 2> /dev/null

##############################################################################
# Link tests

.PHONY: tests test

tests test: $(OUT_TESTS)

$(OUT_TESTS): $(OUT_STATIC) $(OBJ_TESTS_SHX) $(OBJ_TESTS)
	$(MAKE) --no-print-directory plugins
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	@mkdir -p $(LOGDIR)
	@touch $(BINDIR)/file.exists > /dev/null
	$(CC) -o $(OUT_TESTS) $(OBJ_TESTS_SHX) $(OBJ_TESTS) -Iinclude $(LIBSIR_S) $(SIR_LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_TESTS)" 2> /dev/null

##############################################################################
# Build documentation

.PHONY: docs doc

docs doc: $(OUT_STATIC)
	@doxygen Doxyfile
	-@find docs -name '*.png' \
	    -not -path 'docs/res/*' \
	    -not -path 'docs/sources/*' -print | \
	  grep '/' > /dev/null 2>&1 && \
	  find docs -name '*.png' \
	    -not -path 'docs/res/*' \
	    -not -path 'docs/sources/*' -print | \
	  grep -Ev '(docs/sample-terminal.png$$|docs/libsir-alpha.png$$)' | \
	  xargs -I {} -P \
	    "$$(getconf NPROCESSORS_ONLN 2> /dev/null || \
	        getconf _NPROCESSORS_ONLN 2> /dev/null || \
	        nproc 2> /dev/null || \
	        printf '%s\n' \
	          "$$(grep -E '^processor[[:space:]].*[0-9]+$$' \
	              /proc/cpuinfo 2> /dev/null | wc -l 2> /dev/null | \
	              tr -cd '0-9\n' 2> /dev/null)" 2> /dev/null || \
	        printf '%s\n' 1)" \
	    $$(printf '%s\n' 'advpng -z4 {}')
	-@printf 'built documentation successfully.\n' 2> /dev/null

##############################################################################
# Run linters

.PHONY: lint check

lint check:
	$(MAKE) --no-print-directory clean
	$(MAKE) --no-print-directory
	$(MAKE) --no-print-directory mcmb
	@rm -rf ./.coverity > /dev/null 2>&1
	@rm -rf ./cov-int > /dev/null 2>&1
	@test -x $(LINTSH) || { \
	  printf '%s\n' "Error: %s not executable.\n" "$(LINTSH)" \
	  2> /dev/null; exit 1; }
	-@printf 'running %s ...\n' "$(LINTSH)"
	@$(LINTSH)

##############################################################################
# Installation

.PHONY: install

install: $(INSTALLSH)
	@test -x $(INSTALLSH) || { \
	  printf 'Error: %s not executable.\n' "$(INSTALLSH)" \
	  2> /dev/null; exit 1; }
	@test -f "$(OUT_STATIC)" || $(MAKE) --no-print-directory static
	@test -f "$(OUT_SHARED)" || $(MAKE) --no-print-directory shared
	-@printf 'installing libraries to %s and headers to %s...' "$(INSTALLLIB)" "$(INSTALLINC)" 2> /dev/null
	$(INSTALLSH) -m 755 -d "$(INSTALLLIB)"
	$(INSTALLSH) -C -m 755 "$(OUT_SHARED)" "$(INSTALLLIB)"
	-($(LDCONFIG) || true) > /dev/null 2>&1
	$(INSTALLSH) -C -m 644 "$(OUT_STATIC)" "$(INSTALLLIB)"
	-($(RANLIB) "$(INSTALLLIB)/$(OUT_STATIC_FN)" || true) > /dev/null 2>&1
	$(INSTALLSH) -m 755 -d "$(INSTALLINC)"
	$(INSTALLSH) -m 755 -d "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir.h" "$(INSTALLINC)"
	$(INSTALLSH) -C -m 644 "include/sir/ansimacros.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/config.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/console.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/defaults.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/errors.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/filecache.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/filesystem.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/helpers.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/impl.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/internal.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/maps.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/mutex.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/platform.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/platform_importc.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/platform_orangec.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/platform_embarcadero.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/plugins.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/textstyle.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/types.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/version.h" "$(INSTALLINC)/sir"
	-@printf 'installed libsir successfully.\n' 2> /dev/null

##############################################################################
# Cleanup

.PHONY: clean distclean

clean distclean:
	@rm -rf $(BUILDDIR) > /dev/null 2>&1 || true
	@rm -rf ./src/*.ln > /dev/null 2>&1 || true
	@rm -rf $(LOGDIR) > /dev/null 2>&1 || true
	@rm -rf ./*.log > /dev/null 2>&1 || true
	@rm -rf ./*.ln > /dev/null 2>&1 || true
	@rm -rf ./*.d > /dev/null 2>&1 || true
	@$(SHELL) -c "set +e > /dev/null 2>&1; \
	for i in $(foreach X,$(wildcard $(BINDINGSDIR)/*/Makefile),$(X)); do \
	  $(MAKE) --no-print-directory -C \"\$$(dirname \$${i:?})\" clean; \
	done"
	-@printf 'build artifacts cleaned successfully.\n' 2> /dev/null

##############################################################################
# Rebuild all plugins

ifneq ($(SIR_NO_PLUGINS),1)

.PHONY: plugins plugin

plugins plugin:
	@$(MAKE) -q --no-print-directory $(OUT_SHARED) $(TUS) || \
	  $(MAKE) --no-print-directory \
	    $(foreach V,$(sort $(strip $(PLUGINNAMES))), $(PLUGPREFIX)$V) REMAKE=1
	@$(MAKE) --no-print-directory $(foreach V,$(sort $(strip $(PLUGINNAMES))), $(PLUGPREFIX)$V)

##############################################################################
# Build a specific plugin

.PHONY: $(PLUGPREFIX)%

$(PLUGPREFIX)%: $(OUT_SHARED) $(TUS)
	@$(MAKE) -q --no-print-directory $(OUT_SHARED) $(TUS) || export REMAKE=1; \
	test -f $(LIBDIR)/$@$(PLATFORM_DLL_EXT) || export REMAKE=1; \
	test $${REMAKE:-0} -eq 0 || { \
	  (set -x; $(CC) $(SIR_SHARED) -o $(LIBDIR)/$@$(PLATFORM_DLL_EXT) $(SIR_CFLAGS) \
	    $(SIR_CSTD) $(wildcard $(subst $(PLUGPREFIX),$(PLUGINS)/,$@)/*.c) $(SIR_LDFLAGS)) && \
	  printf 'built %s successfully.\n' "$(LIBDIR)/$@$(PLATFORM_DLL_EXT)" 2> /dev/null; }

endif # ifneq ($(SIR_NO_PLUGINS),1)

##############################################################################
# Common rules

-include sircommon.mk

##############################################################################
