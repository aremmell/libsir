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
  test "no_cppi" != "1" > /dev/null 2>&1 && {
      printf '%s' "Formatting with cppi ... "
      ( # shellcheck disable=SC2038
        find . -name "*.[ch]" | xargs -I{} "${SHELL:-sh}" -c \
            'set -e; cppi "{}" > "{}.cppi" && mv -f "{}.cppi" "{}"'
      ) && printf '%s' "complete."
  }; printf '%s\n' ""
}

########################################################################
# Enable tool(s)

# shellcheck disable=SC2043
for n in cppi; do
  command -v "${n:?}" > /dev/null 2>&1 || {
      printf '%s\n' "${n:?} not found!"
      eval set  "_no_${n:?}"=1; }
  test "_no_${n:-}" -eq "1" > /dev/null 2>&1 || {
      command -v "run_${n:?}" > /dev/null 2>&1 &&
          export _tools="${n:?} ${_tools:-}"; }
done

########################################################################
# Run tool(s)

for func in ${_tools:-}; do
  command -v "run_${func:?}" > /dev/null 2>&1 && ("run_${func:?}")
done

########################################################################
