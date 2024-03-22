#!/usr/bin/env sh

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
# Check for csh as sh

# shellcheck disable=SC2006,SC2046,SC2065,SC2116
test _`echo asdf 2>/dev/null` != _asdf >/dev/null &&\
    printf '%s\n' "Error: csh as sh is not supported." &&\
    exit 1

################################################################################
# Check directory

# shellcheck disable=SC2065
test -f "./${0##*/}" > /dev/null 2>&1 ||
  {
    printf '%s\n' "Error: cannot locate script in current directory."
    exit 1
  }

################################################################################
# Fail early on pipelines, if possible

# shellcheck disable=SC3040
(set -o pipefail > /dev/null 2>&1) && set -o pipefail

################################################################################
# Cppi

run_cppi()
{
  printf '%s' "Formatting with cppi ..."
  ( # shellcheck disable=SC2038
    find . -name "*.[ch]" -o -name "*.cc" -o -name "*.hh" | \
      grep -Ev '(build/.*|mcmb\.c$|\.git/.*)' | \
        xargs -I{} "${SHELL:-sh}" -c \
          'set -e; cppi "{}" > "{}.cppi" && mv -f "{}.cppi" "{}"'
  ) && printf '%s\n' "complete."
}

################################################################################
# Run tool(s)

command -v cppi > /dev/null 2>&1 ||
  {
    printf '%s\n' "Error: Could not locate cppi."
    exit 1
  }
run_cppi

################################################################################
