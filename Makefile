# Makefile

################################################################################
#
# Version: 2.2.5
#
##############################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
# Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
##############################################################################

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
WARNEXTRA   ?= -Wextra

##############################################################################
# Flags

SIR_CFLAGS  := $(CFLAGS)
SIR_LDFLAGS := $(LDFLAGS)
SIR_SHFLAGS  = $(subst -static,,$(SIR_LDFLAGS))

##############################################################################
# Platform specifics

include sirplatform.mk

##############################################################################
# C standard

SIR_CSTD ?= -std=c11
SIR_GSTD ?= -std=gnu11
SIR_XSTD ?= -std=c++20

##############################################################################
# Base CFLAGS

ifneq ($(NO_DEFAULT_CFLAGS),1)
  SIR_CFLAGS += -Wall $(strip $(WARNEXTRA) $(WPEDANTIC) $(SIR_FPIC) -Iinclude)
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
# Disable shared library support?

ifeq ($(SIR_NO_SHARED),1)
  SIR_NO_PLUGINS = 1
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
# Disable ANSI text style encoding?

ifeq ($(SIR_NO_TEXT_STYLING),1)
  SIR_CFLAGS += -DSIR_NO_TEXT_STYLING
endif

#############################################################################
# Use CRLF line endings?

ifeq ($(SIR_USE_EOL_CRLF),1)
  SIR_CFLAGS += -DSIR_USE_EOL_CRLF
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

ifeq ($(SIR_NO_SHARED),1)
  ALL_GOALS = $(PGOALS) $(OUT_STATIC) $(OUT_EXAMPLE) $(OUT_TESTS)
else
  ALL_GOALS = $(PGOALS) $(OUT_SHARED) $(OUT_STATIC) $(OUT_EXAMPLE) $(OUT_TESTS)
endif

.PHONY: all

all: $(ALL_GOALS)

##############################################################################
# Automatic dependencies

ifneq (,$(wildcard $(INTDIR)/*/*.d))
  include $(wildcard $(INTDIR)/*/*.d)
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

SIR_XFLAGS := $(strip $(patsubst $(SIR_CXFLAGS), ,$(SIR_CFLAGS) $(CXXFLAGS)))

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
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[shared] built %s successfully.\n' "$(OUT_SHARED)" 2> /dev/null
	-@tput sgr0 2> /dev/null || true

##############################################################################
# Link static library

.PHONY: static

static: $(OUT_STATIC)

$(OUT_STATIC): $(OBJ_STATIC)
	@mkdir -p $(@D)
	$(AR_CR) $(OUT_STATIC) $(OBJ_STATIC)
	-@($(RANLIB) "$(OUT_STATIC)" || true) > /dev/null 2>&1
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[static] built %s successfully.\n' "$(OUT_STATIC)" 2> /dev/null
	-@tput sgr0 2> /dev/null || true

##############################################################################
# Link example

.PHONY: example

example: $(OUT_EXAMPLE)

$(OUT_EXAMPLE): $(OUT_STATIC) $(OBJ_EXAMPLE)
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	$(CC) -o $(OUT_EXAMPLE) $(OBJ_EXAMPLE) -Iinclude -L$(LIBDIR) $(LIBSIR_S) $(SIR_LDFLAGS)
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[example] built %s successfully.\n' "$(OUT_EXAMPLE)" 2> /dev/null
	-@tput sgr0 2> /dev/null || true

##############################################################################
# Link miniature combinatorics utility

.PHONY: mcmb cmb

mcmb cmb: $(OUT_MCMB)

$(OUT_MCMB): $(OBJ_MCMB)
	mkdir -p $(@D)
	mkdir -p $(BINDIR)
	$(CC) -o $(OUT_MCMB) $(OBJ_MCMB) $(SIR_LDFLAGS)
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[mcmb] built %s successfully.\n' "$(OUT_MCMB)" 2> /dev/null
	-@tput sgr0 2> /dev/null || true

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
	$(CXX) -o $(OUT_TESTSXX) $(OBJ_TESTS_SHX) $(OBJ_TESTSXX) -Iinclude -L$(LIBDIR) $(LIBSIR_S) $(SIR_LDFLAGS)
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[tests++] built %s successfully.\n' "$(OUT_TESTSXX)" 2> /dev/null
	-@tput sgr0 2> /dev/null || true

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
	$(CC) -o $(OUT_TESTS) $(OBJ_TESTS_SHX) $(OBJ_TESTS) -Iinclude -L$(LIBDIR) $(LIBSIR_S) $(SIR_LDFLAGS)
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[tests] built %s successfully.\n' "$(OUT_TESTS)" 2> /dev/null
	-@tput sgr0 2> /dev/null || true

##############################################################################
# Build documentation

.PHONY: docs doc

docs doc: $(OUT_STATIC) GTAGS
	@doxygen Doxyfile
	@rm -rf docs/HTML/* > /dev/null 2>&1 || true
	@env GTAGSCONF="$$(pwd)/docs/res/gtags.conf" htags --map-file \
	    --auto-completion --colorize-warned-line -I -h --tabs 4 \
	    -t "libsir: The Standard Incident Reporter library" \
	    --show-position -n -o -s --table-flist docs
	@cp -f "docs/res/style.css" "docs/HTML/"
	@cp -f "docs/res/libsir-icon192.png" "docs/HTML/icons/pglobe.png"
	@sed \
	   -e "s/http:\/\/www.gnu.org\/software\/global\//https:\/\/github.com\/aremmell\/libsir/" \
	   -e "s/ title='Go to the GLOBAL project page.'//" \
	   -e "s/ alt='\[Powered by GLOBAL-.*\]'//" \
	   -e "s/^<a href='http/<br \/><a href='http/" \
	       "docs/HTML/index.html" > "docs/HTML/index.html.tmp" && \
	 mv -f "docs/HTML/index.html.tmp" "docs/HTML/index.html" && \
	 cp -f "docs/HTML/index.html" "docs/HTML/mains.html"
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
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[docs] built documentation successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

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
	   (tput bold 2> /dev/null || true; tput setaf 1 2> /dev/null || true); \
	    printf '%s\n' "Error: %s not executable.\n" "$(LINTSH)" \
	      2> /dev/null; (tput sgr0 2> /dev/null || true); exit 1; }
	-@tput bold 2> /dev/null || true; tput setaf 3 2> /dev/null || true
	-@printf '[lint] running %s ...\n' "$(LINTSH)"
	-@tput sgr0 2> /dev/null || true
	@$(LINTSH)
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[lint] tests completed successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

##############################################################################
# Installation

.PHONY: install

install: $(INSTALLSH)
	@test -x $(INSTALLSH) || { \
	   (tput bold 2> /dev/null || true; tput setaf 1 2> /dev/null || true); \
	    printf 'Error: %s not executable.\n' "$(INSTALLSH)" \
	      2> /dev/null; (tput sgr0 2> /dev/null || true); exit 1; }
	@test -f "$(OUT_STATIC)" || $(MAKE) --no-print-directory static
ifneq ($(SIR_NO_SHARED),1)
	@test -f "$(OUT_SHARED)" || $(MAKE) --no-print-directory shared
endif
	-@printf '[install] installing libraries to %s and headers to %s...' "$(INSTALLLIB)" "$(INSTALLINC)" 2> /dev/null
	$(INSTALLSH) -m 755 -d "$(INSTALLLIB)"
ifneq ($(SIR_NO_SHARED),1)
	$(INSTALLSH) -C -m 755 "$(OUT_SHARED)" "$(INSTALLLIB)"
	-($(LDCONFIG) || true) > /dev/null 2>&1
endif
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
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[install] installed libsir successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

##############################################################################
# Cleanup

.PHONY: distclean

distclean: clean
	@rm -rf ./docs/HTML > /dev/null 2>&1 || true
	@rm -f ./GRTAGS > /dev/null 2>&1 || true
	@rm -f ./GPATH > /dev/null 2>&1 || true
	@rm -f ./GTAGS > /dev/null 2>&1 || true
	@rm -f ./TAGS > /dev/null 2>&1 || true
	@rm -f ./tags > /dev/null 2>&1 || true

.PHONY: clean clean-all
clean clean-all:
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
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[clean] libsir cleaned successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

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
	  (tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true) && \
	  printf '[plugin] built %s successfully.\n' "$(LIBDIR)/$@$(PLATFORM_DLL_EXT)" 2> /dev/null; \
	  (tput sgr0 2> /dev/null || true); }

endif # ifneq ($(SIR_NO_PLUGINS),1)

##############################################################################
# Tags

.PHONY: ctags tags TAGS GPATH GRTAGS GTAGS tag

ctags tags TAGS GPATH GRTAGS GTAGS tag:
	-@rm -f tags TAGS GPATH GRTAGS GTAGS > /dev/null 2>&1 || true; \
	  FDIRS="LICENSE Makefile bindings/python/*.py *.mk *.md bindings example include plugins src tests"; \
	  FLIST="$$(2> /dev/null find $${FDIRS} | \
	            2> /dev/null xargs -I{} \
	            2> /dev/null printf %s\\n \"{}\" | \
	            2> /dev/null xargs)"; \
	  etags $${FLIST:-} > /dev/null 2>&1; \
	  ctags -e $${FLIST:-} > /dev/null 2>&1; \
	  ctags $${FLIST:-} > /dev/null 2>&1; \
	  printf %s\\n $${FLIST:-} 2> /dev/null | \
	      env GTAGSCONF="$$(pwd)/docs/res/gtags.conf" \
	        gtags -f - > /dev/null 2>&1; \
	  ls tags TAGS GPATH GRTAGS GTAGS 2> /dev/null | \
	      grep -q '.' 2> /dev/null && { \
	  (tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true) && \
	  printf '[tags] regenerated %s successfully.\n' "tags" 2> /dev/null; \
	  (tput sgr0 2> /dev/null || true); exit 0; } || { \
	  (tput bold 2> /dev/null || true; tput setaf 1 2> /dev/null || true) && \
	  printf '[tags] failed to regenerate %s.\n' "tags" 2> /dev/null; \
	  (tput sgr0 2> /dev/null || true); } ; exit 1

##############################################################################
# Common rules

include sircommon.mk

##############################################################################
