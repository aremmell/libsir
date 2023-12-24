# bindings/python/Makefile

################################################################################
#
# Version: 2.2.5
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
# Defaults

.NOTPARALLEL:
RANLIB   ?= ranlib
AR       ?= ar

################################################################################
# Platform specifics

include ../../sirplatform.mk

################################################################################
# Targets

.PHONY: all

all:
	@$(MAKE) --no-print-directory check_main
	@$(MAKE) --no-print-directory module-python
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[binding] built python binding successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

################################################################################
# Python module

.PHONY: module-python

module-python: sir_wrap.c sir.py
	env LDFLAGS="$(LDFLAGS) $(SIR_LDFLAGS)" \
	  CC="$(CC) $(OPTFLAGS) $(CFLAGS) $(SIR_CFLAGS)" \
	python3 setup.py build_ext --inplace

################################################################################
# Swig

sir.py sir_wrap.c: sir.i
	env LDFLAGS="$(LDFLAGS) $(SIR_LDFLAGS)" \
	  CC="$(CC) $(OPTFLAGS) $(CFLAGS) $(SIR_CFLAGS)" \
	swig -O -w451 -python -threads -outcurrentdir -v $<
	-@tput setaf 6 2> /dev/null || true
	-@printf '[binding] generated wrappers.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

################################################################################
# Check main build files

.PHONY: check_main

check_main:
	@test -f ../../sirplatform.mk 2> /dev/null \
	  || { \
	       (tput bold 2> /dev/null || true; tput setaf 1 2> /dev/null || true); \
	       printf '%s\n' "ERROR: Invalid working directory."; \
	       (tput sgr0 2> /dev/null || true); \
	       exit 1; \
	  }
	@$(MAKE) --no-print-directory -q -C ../.. 2> /dev/null \
	  || { \
	       $(MAKE) --no-print-directory clean 2> /dev/null; \
	       $(MAKE) --no-print-directory -C ../.. clean 2> /dev/null; \
	       $(MAKE) --no-print-directory -C ../..; \
	  }

################################################################################
# Clean-up

.PHONY: clean-all distclean

clean-all distclean:
	@$(MAKE) --no-print-directory -C ../.. clean

.PHONY: clean

clean:
	@rm -f ./sir-python.log 2> /dev/null || true
	@rm -f ./_sir.*.dylib 2> /dev/null || true
	@rm -rf ./__pycache__ 2> /dev/null || true
	@rm -f ./sir_wrap.c 2> /dev/null || true
	@rm -f ./check_main 2> /dev/null || true
	@rm -f ./_sir.*.so 2> /dev/null || true
	@rm -f ./src/*.obj 2> /dev/null || true
	@rm -f ./src/*.o 2> /dev/null || true
	@rm -rf ./build 2> /dev/null || true
	@rm -f ./sir.py 2> /dev/null || true
	@rmdir ./src 2> /dev/null || true
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[clean] bindings/python cleaned successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

################################################################################
# Common rules

include ../../sircommon.mk

################################################################################
