# sircommon.mk

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

################################################################################
# Print a specific make variable

.PHONY: print-%

print-%:
	-@$(info $*: [$($*)] ($(flavor $*). set by $(origin $*)))@true
	-@true > /dev/null 2>&1

################################################################################
