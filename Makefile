#
# libsir -- Makefile
# https://github.com/aremmell/libsir
#
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman
#

SHELL      := $(shell env sh -c 'PATH="$$(command -p getconf PATH)" command -v sh')
BUILDDIR    = ./build
LOGDIR      = ./logs
LINTSH      = ./.lint.sh
DOCSDIR     = docs
TESTS       = tests
EXAMPLE     = example
UTILS       = utils
MCMB        = mcmb
INTDIR      = $(BUILDDIR)/obj
LIBDIR      = $(BUILDDIR)/lib
BINDIR      = $(BUILDDIR)/bin
PREFIX     ?= /usr/local
INSTALLLIB  = $(DESTDIR)$(PREFIX)/lib
INSTALLINC  = $(DESTDIR)$(PREFIX)/include
INSTALLSH   = ./build-aux/install-sh
RANLIB     ?= ranlib
LDCONFIG   ?= ldconfig
PLUGINS     = ./plugins
PLUGINNAMES = $(subst $(PLUGINS)/,,$(wildcard $(PLUGINS)/*))
PLUGPREFIX  = plugin_

# platform specifics
include sirplatform.mk

# C standard
SIR_CSTD ?= -std=c11
SIR_GSTD ?= -std=gnu11

# base CFLAGS
ifneq ($(NO_DEFAULT_CFLAGS),1)
  CFLAGS += -Wall -Wextra -Wpedantic -Iinclude -fPIC
endif

# debug/non-debug CFLAGS
ifeq ($(SIR_DEBUG),1)
 DBGFLAG ?= -g3
  CFLAGS += $(DBGFLAG) -O0 -DDEBUG -U_FORTIFY_SOURCE
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

# disable plugins?
ifeq ($(SIR_NO_PLUGINS),1)
  CFLAGS += -DSIR_NO_PLUGINS
else
  LIBDL  ?= -ldl
  PGOALS  = plugins
endif

# dependencies
LIBS = $(PTHOPT)

# linker flags
LDFLAGS += $(LIBS) -L$(LIBDIR) $(PLATFORM_LIBS) $(LIBDL) $(EXTRA_LIBS)

# static libsir for test rig and example
LIBSIR_S = -lsir_s

# translation units
TUS := $(wildcard src/*.c)

# intermediate files
_OBJ = $(strip $(patsubst %.c, %.o, $(TUS)))
OBJ  = $(strip $(patsubst %, $(INTDIR)/%, $(_OBJ)))

# shared library
OBJ_SHARED     = $(strip $(patsubst %.o, $(INTDIR)/%.o, $(_OBJ)))
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

# miniature combinatorics utility
OBJ_MCMB       = $(INTDIR)/$(MCMB)/$(MCMB).o
OUT_MCMB       = $(BINDIR)/mcmb$(PLATFORM_EXE_EXT)

# ##########
# targets
# ##########

.DEFAULT_GOAL := all
.PHONY: all
all: $(PGOALS) $(OUT_SHARED) $(OUT_STATIC) $(OUT_EXAMPLE) $(OUT_TESTS) $(OUT_MCMB)

-include $(INTDIR)/*.d

$(OBJ_EXAMPLE): $(EXAMPLE)/$(EXAMPLE).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) $(SIR_CSTD) -Iinclude

$(OBJ_MCMB): $(UTILS)/$(MCMB)/$(MCMB).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) $(SIR_GSTD)

$(OBJ_TESTS): $(TESTS)/$(TESTS).c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) $(SIR_CSTD) -Iinclude

$(INTDIR)/%.o: %.c $(DEPS)
	@mkdir -p $(@D)
	$(CC) $(MMDOPT) -c -o $@ $< $(CFLAGS) $(SIR_CSTD)

.PHONY: shared
shared: $(OUT_SHARED)
$(OUT_SHARED): $(OBJ_SHARED)
	@mkdir -p $(@D)
	$(CC) $(SIR_SHARED) -o $(OUT_SHARED) $^ $(LDFLAGS_SHARED)
	-@printf 'built %s successfully.\n' "$(OUT_SHARED)" 2> /dev/null

.PHONY: static
static: $(OUT_STATIC)
$(OUT_STATIC): $(OUT_SHARED)
	@mkdir -p $(@D)
	ar -cr $(OUT_STATIC) $(OBJ_SHARED)
	-@($(RANLIB) "$(OUT_STATIC)" || true) > /dev/null 2>&1
	-@printf 'built %s successfully.\n' "$(OUT_STATIC)" 2> /dev/null

.PHONY: example
example: $(OUT_EXAMPLE)
$(OUT_EXAMPLE): $(OUT_STATIC) $(OBJ_EXAMPLE)
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	$(CC) -o $(OUT_EXAMPLE) $(OBJ_EXAMPLE) -Iinclude $(LIBSIR_S) $(LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_EXAMPLE)" 2> /dev/null

.PHONY: mcmb cmb
mcmb cmb: $(OUT_MCMB)
$(OUT_MCMB): $(OBJ_MCMB)
	mkdir -p $(@D)
	mkdir -p $(BINDIR)
	$(CC) -o $(OUT_MCMB) $(OBJ_MCMB) $(LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_MCMB)" 2> /dev/null

.PHONY: tests test
tests test: $(OUT_TESTS)
$(OUT_TESTS): $(OUT_STATIC) $(OBJ_TESTS)
	$(MAKE) --no-print-directory plugins
	@mkdir -p $(@D)
	@mkdir -p $(BINDIR)
	@mkdir -p $(LOGDIR)
	@touch $(BINDIR)/file.exists > /dev/null
	$(CC) -o $(OUT_TESTS) $(OBJ_TESTS) -Iinclude $(LIBSIR_S) $(LDFLAGS)
	-@printf 'built %s successfully.\n' "$(OUT_TESTS)" 2> /dev/null

.PHONY: docs doc
docs doc: $(OUT_STATIC)
	@doxygen Doxyfile
	-@find docs -name '*.png' \
		-not -path 'docs/res/*' \
		-not -path 'docs/sources/*' \
		-exec advpng -z4 "{}" 2> /dev/null \; \
		2> /dev/null || true
	-@printf 'built documentation successfully.\n' 2> /dev/null

.PHONY: lint check
lint check:
	$(MAKE) --no-print-directory clean
	$(MAKE) --no-print-directory
	$(MAKE) --no-print-directory mcmb
	@rm -rf ./.coverity > /dev/null 2>&1
	@rm -rf ./cov-int > /dev/null 2>&1
	@test -x $(LINTSH) || \
		{ printf '%s\n' "Error: %s not executable.\n" "$(LINTSH)" \
			2> /dev/null; exit 1; }
	-@printf 'running %s ...\n' "$(LINTSH)"
	@$(LINTSH)

.PHONY: install
ifneq (,$(findstring install,$(MAKECMDGOALS)))
.NOTPARALLEL:
endif
install: $(INSTALLSH)
	@test -x $(INSTALLSH) || \
		{ printf 'Error: %s not executable.\n' "$(INSTALLSH)" \
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
	$(INSTALLSH) -C -m 644 "include/sir/plugins.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/textstyle.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/types.h" "$(INSTALLINC)/sir"
	$(INSTALLSH) -C -m 644 "include/sir/version.h" "$(INSTALLINC)/sir"
	-@printf 'installed libsir successfully.\n' 2> /dev/null

.PHONY: clean distclean
ifneq (,$(findstring clean,$(MAKECMDGOALS)))
.NOTPARALLEL:
endif
clean distclean:
	@rm -rf $(BUILDDIR) > /dev/null 2>&1
	@rm -rf $(LOGDIR) > /dev/null 2>&1
	@rm -rf ./*.log > /dev/null 2>&1
	@rm -rf ./*.ln > /dev/null 2>&1
	@rm -rf ./*.d > /dev/null 2>&1
	-@printf 'build directory and log files cleaned successfully.\n' 2> /dev/null

ifneq ($(SIR_NO_PLUGINS),1)
.PHONY: plugins plugin
plugins plugin:
	@$(MAKE) -q --no-print-directory $(OUT_SHARED) $(TUS) || \
		$(MAKE) --no-print-directory \
			$(foreach V,$(sort $(strip $(PLUGINNAMES))), $(PLUGPREFIX)$V) REMAKE=1
	@$(MAKE) --no-print-directory $(foreach V,$(sort $(strip $(PLUGINNAMES))), $(PLUGPREFIX)$V)

.PHONY: $(PLUGPREFIX)%
$(PLUGPREFIX)%: $(OUT_SHARED) $(TUS)
	@$(MAKE) -q --no-print-directory $(OUT_SHARED) $(TUS) || export REMAKE=1; \
	test -f $(LIBDIR)/$@$(PLATFORM_DLL_EXT) || export REMAKE=1; \
	test $${REMAKE:-0} -eq 0 || \
		{ (set -x; $(CC) $(SIR_SHARED) -o $(LIBDIR)/$@$(PLATFORM_DLL_EXT) $(CFLAGS) \
		   $(SIR_CSTD) $(wildcard $(subst $(PLUGPREFIX),$(PLUGINS)/,$@)/*.c) \
		   $(LDFLAGS_SHARED)) && \
	printf 'built %s successfully.\n' "$(LIBDIR)/$@$(PLATFORM_DLL_EXT)" 2> /dev/null; }
endif

.PHONY: printvars printenv
printvars printenv:
	-@printf '%s: ' "FEATURES" 2> /dev/null
	-@printf '%s ' "$(.FEATURES)" 2> /dev/null
	-@printf '%s\n' "" 2> /dev/null
	-@$(foreach V,$(sort $(.VARIABLES)), \
		$(if $(filter-out environment% default automatic,$(origin $V)), \
			$(if $(strip $($V)),$(info $V: [$($V)]),)))
	-@true > /dev/null 2>&1

.PHONY: print-%
print-%:
	-@$(info $*: [$($*)] ($(flavor $*). set by $(origin $*)))@true
	-@true > /dev/null 2>&1
