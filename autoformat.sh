#!/usr/bin/env sh

########################################################################

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

########################################################################
# Sanity checks

# Check for csh as sh
# shellcheck disable=SC2006,SC2046,SC2065,SC2116
test _`echo asdf 2>/dev/null` != _asdf >/dev/null &&\
    printf '%s\n' "Error: csh as sh is not supported." &&\
    exit 1

# Check directory
# shellcheck disable=SC2065
test -f "./${0##*/}" > /dev/null 2>&1 || {
    printf '%s\n' "Error: cannot locate script in current directory."
    exit 1
}

########################################################################
# Fail early on pipelines, if possible

# shellcheck disable=SC3040
(set -o pipefail > /dev/null 2>&1) && set -o pipefail

########################################################################
# Cppi

run_cppi()
{
  printf '%s' "Formatting with cppi ... "
  ( # shellcheck disable=SC2038
    find . -name "*.[ch]" | xargs -I{} "${SHELL:-sh}" -c \
        'set -e; cppi "{}" > "{}.cppi" && mv -f "{}.cppi" "{}"'
  ) && printf '%s\n' "complete."
}

########################################################################
# Run tool(s)

command -v cppi > /dev/null 2>&1 ||
  {
    printf '%s\n' "Error: Could not locate cppi."
    exit 1
  }
run_cppi

########################################################################
