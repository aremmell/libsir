# bindings/ch/Makefile
# libsir: https://github.com/aremmell/libsir
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

##############################################################################

.NOTPARALLEL:

# TODO: Windows needs manual linking, like:
#lib.dl: chdl.obj func.obj
#        ch dllink lib.dl chdl.obj func.lib
#chdl.obj: chdl.c
#        ch dlcomp lib.dl chdl.c -I.
#func.obj: func.c
#        cl /c /Fofunc.obj func.c
#        link /DLL /OUT:func.dll func.obj

.PHONY: all
all: target

GEN_FUNCS=sir_alert.chf sir_crit.chf sir_debug.chf sir_emerg.chf \
          sir_error.chf sir_info.chf sir_notice.chf sir_warn.chf

$(GEN_FUNCS): sir_output.chi
	@$${SED:-sed} "s/SIR_OUTPUT/$$(printf '%s\n' "$(@)" | \
           $${SED:-sed} 's/\.chf//')/g" "sir_output.chi" > "$(@)" && \
         ( printf 'Generated %s\n' "$(@)" 2> /dev/null || true )

.PHONY: target
target: libsir.dl

libsir.dl: sir_chdl.o chsir.h ../../build/lib/libsir_s.a $(GEN_FUNCS)
	ch dllink libsir.dl sir_chdl.o ../../build/lib/libsir_s.a

sir_chdl.o: sir_chdl.c chsir.h
	ch dlcomp libsir.dl sir_chdl.c -I. -I../../include -D_CH_BUILD

chsir.h: ../../include/sir.h

.PHONY: clean
clean:
	rm -f $(GEN_FUNCS)
	rm -f ./*.o ./*.dl ./*.obj ./*.exp ./*.lib ./*.dll
