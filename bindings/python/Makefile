# bindings/python/Makefile
# libsir: https://github.com/aremmell/libsir
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

##############################################################################
# Python binding

MKFILE_PATH:=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
CURRENT_DIR:=$(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
SIR_BINDINGS:=$(strip $(SIR_BINDINGS) $(BINDINGDIR)/$(CURRENT_DIR))
export SIR_BINDINGS

##############################################################################
# Targets

.PHONY: all
all: swig-$(BINDINGDIR)/$(CURRENT_DIR)

.PHONY: clean
clean: clean-$(BINDINGDIR)/$(CURRENT_DIR)

##############################################################################
# Swig

.PHONY: swig-$(BINDINGDIR)/$(CURRENT_DIR)
swig-$(BINDINGDIR)/$(CURRENT_DIR):
	@$(MAKE) module-$(CURRENT_DIR)

../$(CURRENT_DIR)/sir.py ../$(CURRENT_DIR)/sir_wrap.c: sir.i
	swig -python -threads -outcurrentdir -v $<

##############################################################################
# Python module

.PHONY: module-$(CURRENT_DIR)
module-$(CURRENT_DIR): ../$(CURRENT_DIR)/sir_wrap.c ../$(CURRENT_DIR)/sir.py
	 @python_options="setup.py build_ext --inplace"; \
	  python $${python_options} && \
	  printf '%s\n' "$(CURRENT_DIR) module built successfully!"

##############################################################################
# Clean-up

.PHONY: clean-$(BINDINGDIR)/$(CURRENT_DIR)
clean-$(BINDINGDIR)/$(CURRENT_DIR):
	-@rm -f ./sir-python.log 2> /dev/null || true
	-@rm -rf ./__pycache__ 2> /dev/null || true
	-@rm -f ./sir_wrap.c 2> /dev/null || true
	-@rm -f ./_sir.*.so 2> /dev/null || true
	-@rm -f ./src/*.obj 2> /dev/null || true
	-@rm -f ./src/*.o 2> /dev/null || true
	-@rm -rf ./build 2> /dev/null || true
	-@rm -f ./sir.py 2> /dev/null || true
	-@rmdir ./src 2> /dev/null || true

##############################################################################
