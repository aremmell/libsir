# bindings/python/Makefile
# libsir: https://github.com/aremmell/libsir
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

##############################################################################
# Defaults

.NOTPARALLEL:
RANLIB   ?= ranlib
LDCONFIG ?= ldconfig
AR       ?= ar

##############################################################################
# Platform specifics

-include ../../sirplatform.mk

##############################################################################
# Targets

.PHONY: all

all:
	@$(MAKE) --no-print-directory check_main
	@$(MAKE) --no-print-directory module-python

##############################################################################
# Python module

.PHONY: module-python

module-python: sir_wrap.c sir.py
	env LDFLAGS="$(LDFLAGS) $(SIR_LDFLAGS)" \
	  CC="$(CC) $(OPTFLAGS) $(CFLAGS) $(SIR_CFLAGS)" \
	python3 setup.py build_ext --inplace

##############################################################################
# Swig

sir.py sir_wrap.c: sir.i
	env LDFLAGS="$(LDFLAGS) $(SIR_LDFLAGS)" \
	  CC="$(CC) $(OPTFLAGS) $(CFLAGS) $(SIR_CFLAGS)" \
	swig -w451 -python -threads -outcurrentdir -v $<

##############################################################################
# Check main build files

.PHONY: check_main

check_main:
	@test -f ../../sirplatform.mk \
	  || { \
	       printf '%s\n' "ERROR: Invalid working directory."; \
	       exit 1; \
	  }
	@$(MAKE) --no-print-directory -q -C ../.. \
	  || { \
	       $(MAKE) --no-print-directory clean; \
	       $(MAKE) --no-print-directory -C ../..; \
	  }

##############################################################################
# Clean-up

.PHONY: clean

clean:
	@rm -f ./sir-python.log 2> /dev/null || true
	@rm -rf ./__pycache__ 2> /dev/null || true
	@rm -f ./sir_wrap.c 2> /dev/null || true
	@rm -f ./check_main 2> /dev/null || true
	@rm -f ./_sir.*.so 2> /dev/null || true
	@rm -f ./src/*.obj 2> /dev/null || true
	@rm -f ./src/*.o 2> /dev/null || true
	@rm -rf ./build 2> /dev/null || true
	@rm -f ./sir.py 2> /dev/null || true
	@rmdir ./src 2> /dev/null || true

##############################################################################
# Common rules

-include ../../sircommon.mk

##############################################################################
