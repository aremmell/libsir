#!/usr/bin/env sh

# Coverity Scan submission script.
################################################################################
#
# Version: 2.2.6
#
##############################################################################
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
##############################################################################

##############################################################################

test "$(uname -s 2> /dev/null || true)" = "Linux" ||
  {
    printf '%s\n' \
      '#### Error: Not running on Linux.'
    exit 1
  }

##############################################################################

uname -mp 2> /dev/null | grep -q -E '(amd64|x86_64)' 2> /dev/null ||
  {
    printf '%s\n' \
      '#### Error: Not running on x86_64 platform.'
    exit 1
  }

##############################################################################

test -d "./.git" ||
  {
    printf '%s\n' \
      '#### Error: Not in top-level git repository.'
    exit 1
  }

##############################################################################

test -x "/bin/sh" ||
  {
    printf '%s\n' \
      '#### Error: No "/bin/sh" executable.'
    exit 1
  }

##############################################################################

tar --version | grep -q "GNU" ||
  {
    printf '%s\n' \
      '#### Error: GNU tar is required.'
    exit 1
  }

##############################################################################

test -z "${COVERITY_TOKEN:-}" &&
  {
    printf '%s\n' \
      '#### Error: COVERITY_TOKEN is unset.'
    exit 1
  }

##############################################################################

# XXX(johnsonjh): Read from environment?
COVERITY_EMAIL="trnsz@pobox.com" && export COVERITY_EMAIL

test -z "${COVERITY_EMAIL:-}" &&
  {
    printf '%s\n' \
      '#### Error: COVERITY_EMAIL is unset.'
    exit 1
  }

##############################################################################

# XXX(johnsonjh): Read from environment?
COVERITY_PROJECT="aremmell/libsir" && export COVERITY_PROJECT

test -z "${COVERITY_PROJECT:-}" &&
  {
    printf '%s\n' \
      '#### Error: COVERITY_PROJECT is unset.'
    exit 1
  }

##############################################################################

# XXX(johnsonjh): Read from environment?
test -z "${COVERITY_DLDIR:-}" &&
  {
    printf '%s\n' \
      '#### Warning: COVERITY_DLDIR is unset, using defaults.'
    COVERITY_DLDIR="${HOME:?}/Downloads/Coverity"
    export COVERITY_DLDIR
  }

##############################################################################

mkdir -p "${COVERITY_DLDIR:?}" ||
  {
    printf '%s\n' \
      "#### Error: Bad ${COVERITY_DLDIR:?} directory."
    exit 1
  }

##############################################################################

wget --version > /dev/null ||
  {
    printf '%s\n' \
      '#### Error: No usable wget in PATH.'
    exit 1
  }

##############################################################################

curl --version > /dev/null ||
  {
    printf '%s\n' \
      '#### Error: No usable curl in PATH.'
    exit 1
  }

##############################################################################

printf '%s\n\n' "Starting in 5s ... "
sleep 5 > /dev/null 2>&1 || true

##############################################################################

printf '#### %s\n' "Begin ${0} (${$})"

##############################################################################

export SHELL=/bin/sh

##############################################################################

set -eu > /dev/null 2>&1

##############################################################################

rm -rf ./cov-int            > /dev/null 2>&1
rm -rf ./.coverity          > /dev/null 2>&1
rm -f  ./libsir-coverity.xz > /dev/null 2>&1

##############################################################################

printf '%s\n' '#### Notice: Downloading latest Coverity Tools checksum.'

wget --quiet --no-check-certificate                                    \
      "https://scan.coverity.com/download/linux64"                     \
     --post-data                                                       \
       "token=${COVERITY_TOKEN:?}&project=${COVERITY_PROJECT:?}&md5=1" \
     -O "${COVERITY_DLDIR:?}/coverity_tool.md5" || true

touch -f "${COVERITY_DLDIR:?}/coverity_tool.md5"
touch -f "${COVERITY_DLDIR:?}/coverity_tool.last.md5"

##############################################################################

cmp -s "${COVERITY_DLDIR:?}/coverity_tool.last.md5" \
       "${COVERITY_DLDIR:?}/coverity_tool.md5" ||
  {
    printf '%s\n' \
      '#### Notice: Update needed due to checksum change.'
    COVERITY_NEED_UPDATE=1
    export COVERITY_NEED_UPDATE=1
  }

##############################################################################

# shellcheck disable=SC2312
test "$(2> /dev/null md5sum "${COVERITY_DLDIR:?}/coverity_tool.tgz" | \
        2> /dev/null cut -d ' ' -f 1)" =                              \
          "$(cat "${COVERITY_DLDIR:?}/coverity_tool.md5" || true)" ||
  {
    test -z "${COVERITY_NEED_UPDATE:-}" && printf '%s\n' \
      '#### Notice: Update needed due to failed verification.'
    COVERITY_NEED_UPDATE=1
    export COVERITY_NEED_UPDATE=1
  }

##############################################################################

test -z "${COVERITY_NEED_UPDATE:-}" ||
  {
    printf '%s\n' '#### Notice: Downloading latest Coverity Tools archive.'
    wget --quiet --no-check-certificate                             \
          "https://scan.coverity.com/download/linux64"              \
         --post-data                                                \
          "token=${COVERITY_TOKEN:?}&project=${COVERITY_PROJECT:?}" \
         -O "${COVERITY_DLDIR:?}/coverity_tool.tgz" || true
  }

##############################################################################

cp -f "${COVERITY_DLDIR:?}/coverity_tool.md5" \
      "${COVERITY_DLDIR:?}/coverity_tool.last.md5" > /dev/null 2>&1 || true

##############################################################################

printf '%s' '#### Notice: Extracting Coverity Tools archive .'
mkdir -p ".coverity" &&                                          \
tar zxf "${COVERITY_DLDIR:?}/coverity_tool.tgz"                  \
    --strip-components=1 -C ".coverity" --checkpoint=5000 2>&1 | \
        awk '{ printf("."); }' && printf '%s\n' " OK."

##############################################################################

printf '%s\n' '#### Notice: Building libsir for Coverity analysis.'
make clean &&                                     \
env TZ=UTC                                        \
    PATH="./.coverity/bin:${PATH:?}"              \
    CXXFLAGS="-DSIR_NO_FMT_FORMAT=1               \
              -DSIR_NO_BOOST_FORMAT=1             \
              -DSIR_NO_STD_FORMAT=1"              \
        ./.coverity/bin/cov-build --dir "cov-int" \
            make all tests++

##############################################################################

printf '%s\n' '#### Notice: Packaging Coverity submission.'
tar caf libsir-coverity.xz cov-int

##############################################################################

printf '%s\n' '#### Notice: Uploading Coverity submission.'
curl --form token="${COVERITY_TOKEN:?}"                      \
     --form email="${COVERITY_EMAIL:?}"                      \
     --form file=@"libsir-coverity.xz"                       \
     --form version="$(date -u "+R9-%s" || true)"            \
     --form description="$(date -u "+LIBSIR-%s" || true)" -k \
  "https://scan.coverity.com/builds?project=${COVERITY_PROJECT:?}"

##############################################################################

printf '%s\n' '#### Notice: Coverity submission complete.'
rm -f libsir-coverity.xz

##############################################################################

printf '#### %s\n' "End ${0} (${$})"

##############################################################################

# vim: filetype=sh:tabstop=2:ai:expandtab
