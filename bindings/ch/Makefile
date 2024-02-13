# bindings/ch/Makefile

################################################################################
#
# Version: 2.2.5
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
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
	@$(MAKE) --no-print-directory module-ch
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[binding] built ch binding successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

################################################################################
# Ch functions

GEN_FUNCS=sir_alert.chf sir_crit.chf sir_debug.chf sir_emerg.chf \
          sir_error.chf sir_info.chf sir_notice.chf sir_warn.chf

$(GEN_FUNCS): sir_output.chi sir_chdl.c chsir.h
	@$${SED:-sed} "s/SIR_OUTPUT/$$(printf '%s\n' "$(@)" | \
           $${SED:-sed} 's/\.chf//')/g" "sir_output.chi" > "$(@)" && \
         ( tput setaf 6 2> /dev/null || true; \
	   printf '[binding] generated %s.\n' "$(@)" 2> /dev/null || true; \
	   tput sgr0 2> /dev/null || true )

################################################################################
# Ch dynamic module

.PHONY: module-ch

module-ch: libsir.dl

libsir.dl: sir_chdl.o chsir.h ../../build/lib/libsir_s.a $(GEN_FUNCS)
	env LDFLAGS="$(LDFLAGS) $(SIR_LDFLAGS)" \
	  CC="$(CC) $(OPTFLAGS) $(CFLAGS) $(SIR_CFLAGS)" \
	ch dllink libsir.dl sir_chdl.o ../../build/lib/libsir_s.a
	@test -f libsir.dl

sir_chdl.o: sir_chdl.c chsir.h
	env LDFLAGS="$(LDFLAGS) $(SIR_LDFLAGS)" \
	  CC="$(CC) $(OPTFLAGS) $(CFLAGS) $(SIR_CFLAGS)" \
	ch dlcomp libsir.dl sir_chdl.c -I. -I../../include -D_CH_BUILD
	@test -f sir_chdl.o

chsir.h: ../../include/sir.h

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
	@rm -f $(GEN_FUNCS)
	@rm -f ./libsir-chdemo.log 2> /dev/null || true
	@rm -f ./_sir.*.dylib 2> /dev/null || true
	@rm -f ./check_main 2> /dev/null || true
	@rm -f ./_sir.*.so 2> /dev/null || true
	@rm -rf ./build 2> /dev/null || true
	@rm -f ./*.obj 2> /dev/null || true
	@rm -f ./*.exp 2> /dev/null || true
	@rm -f ./*.lib 2> /dev/null || true
	@rm -f ./*.dll 2> /dev/null || true
	@rm -f ./*.dl 2> /dev/null || true
	@rm -f ./*.o 2> /dev/null || true
	-@tput bold 2> /dev/null || true; tput setaf 2 2> /dev/null || true
	-@printf '[clean] bindings/ch cleaned successfully.\n' 2> /dev/null
	-@tput sgr0 2> /dev/null || true

################################################################################
# Common rules

include ../../sircommon.mk

################################################################################
