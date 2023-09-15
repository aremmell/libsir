# sircommon.mk
# libsir: https://github.com/aremmell/libsir
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

##############################################################################
# Print all make variables

.PHONY: printvars printenv

printvars printenv:
	-@printf '%s: ' "FEATURES" 2> /dev/null
	-@printf '%s ' "$(.FEATURES)" 2> /dev/null
	-@printf '%s\n' "" 2> /dev/null
	-@$(foreach V,$(sort $(.VARIABLES)), \
	    $(if $(filter-out environment% default automatic,$(origin $V)), \
	    $(if $(strip $($V)),$(info $V: [$($V)]),)))
	-@true > /dev/null 2>&1

##############################################################################
# Print a specific make variable

.PHONY: print-%

print-%:
	-@$(info $*: [$($*)] ($(flavor $*). set by $(origin $*)))@true
	-@true > /dev/null 2>&1

##############################################################################
