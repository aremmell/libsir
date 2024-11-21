#!/usr/bin/env sh
# shellcheck disable=SC2312,SC2317

################################################################################
#
# Version: 2.2.6
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

command -v "${GIT:-git}" > /dev/null 2>&1 \
  || {
    printf '%s\n' "ERROR: Unable to find git command!"
    exit 99
  }

################################################################################

CPUS="$(getconf _NPROCESSORS_ONLN 2> /dev/null || printf '%s\n' '1')" && \
    export CPUS

################################################################################

CCACHE="$(command -v ccache 2> /dev/null || printf '%s\n' 'env')" && \
    export CCACHE

################################################################################

printf '%s\n' "checking for git toplevel ..."
cd "$(${GIT:-git} rev-parse --show-toplevel)" \
  || {
    printf '%s\n' "ERROR: Unable to find git toplevel!"
    exit 99
  }

################################################################################

test_spaces()
{ (
  printf '%s\n' "checking for spaces in filenames ..."
  if (find src include -print | awk '{ print "\""$0"\"" }' | grep ' '); then
    {
      printf '%s\n' "ERROR: Filename check failed due to spaces!"
      exit 99
    }
  fi
  exit 0
) }

################################################################################

test_tabs()
{ (
  printf '%s\n' "checking source code for tabs ..."
  TLIST="$(find src include -print | grep -E '(\.[ch]$|\.cc$|\.hh$)' \
    | xargs -L 1 grep -l "$(printf '\t')" 2> /dev/null)" || true
  # shellcheck disable=SC2015
  printf "%s\n" "${TLIST:-}" | grep -v '^$' 2> /dev/null | grep . \
    && {
      printf '%s\n' "ERROR: Tabs check failed!"
      exit 99
    }
  exit 0
) }

################################################################################

test_whitespace()
{ (
  printf '%s\n' "checking source code for trailing whitespace ..."
  # shellcheck disable=SC2038
  TLIST="$(find src include -print \
    | xargs -I{} grep -al ' \+$' "{}" 2> /dev/null)" || true
  # shellcheck disable=SC2015
  printf "%s\n" "${TLIST:-}" | grep -v '^$' 2> /dev/null | grep . \
    && {
      printf '%s\n' "ERROR: Trailing whitespace check failed!"
      exit 99
    }
  exit 0
) }

################################################################################

test_mcmb()
{
  mkdir -p build/lib > /dev/null 2>&1 || true
  ${MAKE:-make} mcmb; ret="${?}"
  test "${ret}" -ne 0 && exit 99
  printf '%s\n' "checking for ${MCMB:-build/bin/mcmb} executable ..."
  test -x "${MCMB:-build/bin/mcmb}" \
    || {
      printf '%s\n' \
        "NOTICE: ${MCMB:-build/bin/mcmb} is not an executable, aborting ${0}."
      exit 99
    }
  # shellcheck disable=SC2089
  SIR_OPTIONS="SIR_DEBUG=1 \
               SIR_NO_SYSTEM_LOGGERS=1 \
               SIR_NO_PLUGINS=1 \
               SIR_NO_TEXT_STYLING=1 \
               SIR_USE_EOL_CRLF=1 \
               SIR_SELFLOG=1"
  # shellcheck disable=SC2090
  printf '%s\n' "exported SIR_OPTIONS=' ${SIR_OPTIONS} '" | tr -s ' '
  # shellcheck disable=SC2090
  export SIR_OPTIONS
  # shellcheck disable=SC2086
  printf 'mcmb: Configured for %d combinations.\n' \
      "$("${MCMB:-build/bin/mcmb}" -et ${SIR_OPTIONS} 2>&1 | tr -d '\r\n')"
}

################################################################################

test_duma()
{ (
  printf '%s\n' "checking for duma ..."
  DUMA_OS="$(uname -s 2> /dev/null)"
  # shellcheck disable=SC2015
  test "${DUMA_OS:-}" = "Darwin" \
    && {
      printf '%s\n' \
        "NOTICE: Skipping DUMA checks on macOS."
      exit 1
    }
  test "${DUMA_OS:-}" = "Darwin" \
    || {
      command -v duma > /dev/null 2>&1 \
        || {
          printf '%s\n' \
            "NOTICE: DUMA not found, skipping DUMA check."
          exit 1
        }
    }
  command -v g++ > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: G++ is required for DUMA check."
      exit 1
    }
  command -v gcc > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: GCC is required for DUMA check."
      exit 1
    }
      rm -f ./duma*.log
      printf '%s\n' "building with DUMA ..."
      env "${MAKE:-make}" clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env DUMA=1 \
          CC="gcc" \
          EXTRA_LIBS="-L/opt/duma/lib" \
          CFLAGS="-I/opt/duma/include" \
        "${MAKE:-make}" \
          -j 1 \
        SIR_DEBUG=1 \
        SIR_SELFLOG=1; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env DUMA=1 \
          CXX="g++" \
          EXTRA_LIBS="-L/opt/duma/lib" \
          CFLAGS="-I/opt/duma/include" \
        "${MAKE:-make}" tests++ \
          -j 1 \
        SIR_DEBUG=1 \
        SIR_SELFLOG=1; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "running DUMA-enabled example ..."
      env DUMA_OUTPUT_FILE=duma1.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirexample; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env DUMA_OUTPUT_FILE=duma2.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirexample; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "running DUMA-enabled tests ..."
      env DUMA_OUTPUT_FILE=duma3.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env DUMA_OUTPUT_FILE=duma4.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "running DUMA-enabled tests++ ..."
      env DUMA_OUTPUT_FILE=duma5.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests++; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env DUMA_OUTPUT_FILE=duma6.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests++; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "checking DUMA output ..."
      test -f duma1.log || DUMA_FAIL=1
      test -f duma2.log || DUMA_FAIL=1
      test -f duma3.log || DUMA_FAIL=1
      test -f duma4.log || DUMA_FAIL=1
      test -f duma5.log || DUMA_FAIL=1
      test -f duma6.log || DUMA_FAIL=1
      test -z "${DUMA_FAIL:-}" \
        || {
          printf '\n%s\n' "ERROR: DUMA failed to log!"
          exit 99
        }
      DUMAOUT="$(cat ./duma*.log 2> /dev/null \
        | grep DUMA 2> /dev/null \
        | grep -Ev \
            '(^DUMA: .*extra leak|^DUMA: .*Reported [1-9]|^DUMA .*compiled |DUMA: .* alloced from.*duma\.c)' 2> /dev/null
      )"
      test -z "${DUMAOUT:-}" \
        || {
          printf '%s\n' "ERROR: DUMA failed, output follows ..."
          printf '%s\n' "${DUMAOUT:?}"
          exit 99
        }
      rm -f ./duma*.log
      exit 0
) }

################################################################################

test_extra()
{ (
  test_mcmb; ret="${?}"
  test "${ret}" -ne 0 && exit 99
  PATH="$(2> /dev/null dirname \
          "$(2> /dev/null printf '%s\n' \
              "$(2> /dev/null env ls -1 \
                   "$(brew --prefix 2> /dev/null)/Cellar/llvm/"*"/bin/clang" | \
                 2> /dev/null sort -rV | \
                 2> /dev/null head -1 \
              )"\
          )"\
       ):/usr/local/bin:${PATH:-}" 2> /dev/null
  command -v clang++ > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang++ is required for the extra-warning check."
      exit 1
    }
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang is required for the extra-warning check."
      exit 1
    }
      printf '%s\n' "building with extra-warning flags ..."
      env "${MAKE:-make}" clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh
      env CC="${CCACHE:-env} clang" \
         CXX="${CCACHE:-env} clang++" \
        "${MAKE:-make}" all tests++ \
        -j "${CPUS:-1}" \
        mcmb; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s' 'set -e; true' > ./.extra.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
        env CC="${CCACHE:-env} clang"
            CXX="${CCACHE:-env} clang++"
            CFLAGS="-DSIR_LINT=1
                    -Werror
                    -Wassign-enum
                    -Wbad-function-cast
                    -Wconversion
                    -Wdisabled-macro-expansion
                    -Wdocumentation
                    -Wdocumentation-deprecated-sync
                    -Wdocumentation-html
                    -Wdocumentation-pedantic
                    -Wdocumentation-unknown-command
                    -Wdollar-in-identifier-extension
                    -Wdouble-promotion
                    -Wextra-semi-stmt
                    -Wformat=2
                    -Wformat-nonliteral
                    -Winvalid-utf8
                    -Wmissing-prototypes
                    -Wmissing-variable-declarations
                    -Wno-sign-conversion
                    -Wno-string-conversion
                    -Wno-unknown-warning-option
                    -Wshift-overflow
                    -Wstring-conversion
                    -Wswitch-enum
                    -Wvla"
            ${MAKE:-make} all tests++
                -j "${CPUS:-1}" ' | tr '\r' ' ' | tr '\n' ' ' | tr -s ' ' >> ./.extra.sh
      printf '%s\n' ' && true' >> ./.extra.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      chmod a+x ./.extra.sh > /dev/null 2>&1 || true
      sh -x ./.extra.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "set -e;" > ./.extraN.sh
      ret="${?}"; test "${ret:-0}" -eq 99 && exit 99
      sed 's/\&\&/\&\&\n/g' ./.extra.sh | head -3 | tail -1 | \
          sed 's/ SIR_.*=1 &&/ /' >> ./.extraN.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      mv -f ./.extraN.sh ./.extra.sh > /dev/null 2>&1; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      ${MAKE:-make} clean || exit 99
      sh -x ./.extra.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh > /dev/null 2>&1 || true
      exit 0
) }

################################################################################

test_gccextra()
{ (
  test_mcmb; ret="${?}"
  test "${ret}" -ne 0 && exit 99
  gcc --version 2>&1 | grep -qi GCC > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: gcc is required for the gcc extra-warning check."
      exit 1
    }
  g++ --version 2>&1 | grep -qi GCC > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: g++ is required for the gcc extra-warning check."
      exit 1
    }
      printf '%s\n' "building with extra-warning flags ..."
      env "${MAKE:-make}" clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh
      env CC="${CCACHE:-env} gcc" \
         CXX="${CCACHE:-env} g++" \
        "${MAKE:-make}" all tests++ \
        -j 1 \
        mcmb; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s' 'set -e; true' > ./.extra.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
        env CC="${CCACHE:-env} gcc"
            CXX="${CCACHE:-env} g++"
            CFLAGS="-Werror
                    -Wbad-function-cast
                    -Wconversion
                    -Wdouble-promotion
                    -Wformat=2
                    -Wformat-nonliteral
                    -Wformat-overflow
                    -Wformat-truncation=2
                    -Wmissing-declarations
                    -Wmissing-prototypes
                    -Wno-sign-conversion
                    -Wno-string-conversion
                    -Wswitch-enum
                    -Wvla"
            ${MAKE:-make} all tests++
                -j 1 ' | tr '\r' ' ' | tr '\n' ' ' | tr -s ' ' >> ./.extra.sh
      printf '%s\n' ' && true' >> ./.extra.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      chmod a+x ./.extra.sh > /dev/null 2>&1 || true
      sh -x ./.extra.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "set -e;" > ./.extraN.sh
      ret="${?}"; test "${ret:-0}" -eq 99 && exit 99
      sed 's/\&\&/\&\&\n/g' ./.extra.sh | head -3 | tail -1 | \
          sed 's/ SIR_.*=1 &&/ /' >> ./.extraN.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      mv -f ./.extraN.sh ./.extra.sh > /dev/null 2>&1; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      ${MAKE:-make} clean || exit 99
      sh -x ./.extra.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh > /dev/null 2>&1 || true
      exit 0
) }

################################################################################

test_flawfinder()
{ (
  command -v flawfinder > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: flawfinder not found, skipping check."
      exit 1
    }
      # shellcheck disable=SC2015
      printf '%s\n' "running flawfinder check ..."
      FLAWFINDER_OUTPUT="$(flawfinder -C -m 5 -c . 2>&1)"
      printf '%s\n' "${FLAWFINDER_OUTPUT:-}" | grep -q "No hits found" \
        || {
          printf '%s\n' "ERROR: Flawfinder failed, output follows ..."
          printf '%s\n' "${FLAWFINDER_OUTPUT:-}"
          exit 99
        }
      exit 0
) }

################################################################################

test_scanbuild()
{ (
  test_mcmb
  command -v scan-build > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: scan-build not found, skipping check."
      exit 1
    }
      printf '%s\n' "running scan-build check ..."
      env "${MAKE:-make}" clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env CC="${CCACHE:-env} clang" \
          CXX="${CCACHE:-env} clang++" \
          "${MAKE:-make}" \
        -j "${CPUS:-1}" all tests++ \
        mcmb; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s' 'set -e;true' > ./.scan-build.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean && ${MAKE:-make} mcmb &&
         env CC="${CCACHE:-env} clang"
             CXX="${CCACHE:-env} clang++"
           scan-build -no-failure-reports
               --status-bugs
               -enable-checker optin.portability.UnixAPI
               -enable-checker security.FloatLoopCounter
               -enable-checker security.insecureAPI.bcmp
               -enable-checker security.insecureAPI.bcopy
                   ${MAKE:-make} all tests++
                       -j "${CPUS:-1}" ' \
        | tr '\r' ' ' | tr '\n' ' ' | tr -s ' ' >> ./.scan-build.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' ' && true' >> ./.scan-build.sh
      chmod a+x ./.scan-build.sh
      sh -x ./.scan-build.sh; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      rm -f ./.scan-build.sh
      rm -rf ./clang-analyzer 2> /dev/null
      exit 0
) }

################################################################################

test_cppcheck()
{ (
  command -v cppcheck > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: cppcheck not found, skipping check."
      exit 1
    }
  command -v cppcheck-htmlreport > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: cppcheck-htmlreport not found, skipping check."
      exit 1
    }
      printf '%s\n' "running cppcheck check ..."
      ${MAKE:-make} clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      rm -rf ./cppcheck
      rm -f ./cppcheck.xml
      mkdir -p cppcheck; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC2046,SC2155
      export EXTRA_INCLUDES="$(gcc -Wp,-v -x c++ - -fsyntax-only < /dev/null 2>&1 | \
                              grep '^ /' | sed 's/^ /-I/' | awk '{ print $1 }')" || \
          export EXTRA_INCLUDES="-I/usr/include"
      # shellcheck disable=SC2086,SC2046
      cppcheck \
      ${EXTRA_INCLUDES:-I/usr/include} \
      --enable="all" \
      --inline-suppr \
      --inconclusive \
      --check-level=exhaustive \
      --library=posix \
      --platform=unix64 \
      --suppress=checkersReport \
      --suppress=*:/Applications/* \
      --suppress=badBitmaskCheck:include/sir/defaults.h \
      --suppress=comparisonError:tests/tests.c \
      --suppress=*:/Library/Developer/* \
      --suppress=missingIncludeSystem \
      --suppress=readdirCalled \
      --suppress=knownConditionTrueFalse \
      --suppress=unmatchedSuppression \
      --suppress=unreadVariable \
      --suppress=unusedStructMember \
      --suppress=*:/usr/include/* \
      -DCLOCK_REALTIME=1 \
      -DCLOCK_MONOTONIC=6 \
      -D_POSIX_TIMERS=2 \
      --suppress=funcArgOrderDifferent:src/sirinternal.c \
      -D"PRINTF_FORMAT_ATTR(x,y)=" \
      -D"SANITIZE_SUPPRESS(x)=" \
      -DBOOST_GCC_VERSION=80300 \
      -DUCHAR_MAX=0xff \
      -DUINT_MAX=0xffffffff \
      -DULONG_MAX=18446744073709551615 \
      -DUSHRT_MAX=0xffff \
      -D_LIBCPP_CXX03_LANG \
      -D_LIBCPP___COMPARE_SYNTH_THREE_WAY_H \
      -D__CHAR_BIT__=8 \
      -D__CPPCHECK__=1 \
      -D__GLIBC__ \
      -D__GNUC__=13 \
      -D__SIZEOF_SIZE_T__=8 \
      -D__STDC__ \
      -D__linux__ \
      -D__x86_64__ \
      -I/usr/include \
      -Iinclude \
      -j "$(getconf NPROCESSORS_ONLN 2> /dev/null || \
            getconf _NPROCESSORS_ONLN 2> /dev/null || \
            nproc 2> /dev/null || printf '%s\n' 4)" \
      $(find . -name '*.[ch]' -o -name '*.cc' -o -name '*.hh' \
        | grep -Ev '(mcmb\.c|\.git/.*|\.gitlab-ci/.*)') \
          --xml --xml-version=2 2> cppcheck.xml \
        && cppcheck-htmlreport --source-dir="." \
          --report-dir="./cppcheck" \
          --file="cppcheck.xml"
      grep -q '<td>0</td><td>total</td>' ./cppcheck/index.html \
        || {
          printf '%s\n' "ERROR: Cppcheck failed ..."
          exit 99
        }
      rm -rf ./cppcheck
      rm -f ./cppcheck.xml
      printf '%s\n' "Actually, don't bother - no results! :)"
      printf '%s\n' ""
      exit 0
) }

################################################################################

test_pvs()
{ (
  rm -f ./.extra.sh > /dev/null 2>&1 || true
  test_mcmb; ret="${?}"
  test "${ret}" -ne 0 && exit 99
  # shellcheck disable=SC2140
  printf '%s\n' "set -e;" > ./.extra.sh
  ret="${?}"; test "${ret:-0}" -eq 99 && exit 99
  # shellcheck disable=SC2140
  eval ./build/bin/mcmb "${SIR_OPTIONS:?}" | xargs -I{} \
    printf '%s\n' "export PVS_FLAGS=\""{}"\";./.lint.sh pvs_real" | \
    sort -u >> ./.extra.sh
  ret="${?}"; test "${ret:-0}" -eq 99 && exit 99
  printf '%s\n' "export PVS_FLAGS=\"\";./.lint.sh pvs_real" >> ./.extra.sh
  ret="${?}"; test "${ret:-0}" -eq 99 && exit 99
  chmod a+x ./.extra.sh > /dev/null 2>&1 || true
  sh -x ./.extra.sh; ret="${?}"
  rm -f ./.extra.sh > /dev/null 2>&1 || true
  test "${ret:-0}" -ne 0 && exit 99
  exit 0
) }

################################################################################

test_pvs_real()
{ (
  export P_FLAGS="${PVS_FLAGS:-}"
  command -v c++ > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: c++ not found, skipping PVS-Studio checks."
      exit 1
    }
  command -v cc > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: cc not found, skipping PVS-Studio checks."
      exit 1
    }
  command -v bear > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: bear not found, skipping PVS-Studio checks."
      exit 1
    }
  command -v pvs-studio-analyzer > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: pvs-studio-analyzer not found, skipping PVS-Studio check."
      exit 1
    }
  command -v plog-converter > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: plog-converter not found, skipping PVS-Studio checks."
      exit 1
    }
      printf '%s\n' "running PVS-Studio checks ..."
      rm -rf ./pvsreport
      rm -f ./log.pvs
      rm -f ./compile_commands.json
      ${MAKE:-make} clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      ${MAKE:-make} mcmb; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC2086
      env CXX="${CCACHE:-env} c++" \
           CC="${CCACHE:-env} cc" \
              bear -- "${MAKE:-make}" all tests++ ${P_FLAGS:-} \
                 -j "${CPUS:-1}"; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "Running PVS-Studio ..."
      pvs-studio-analyzer analyze --intermodular -j "${CPUS:-1}" -o log.pvs
      test -f log.pvs; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "PVS-Studio run completed ..."
      # shellcheck disable=SC2015
      plog-converter -a "GA:1,2,3;OP:1,2,3;64:1,2,3;CS:1,2,3;MISRA:1,2,3;OWASP:1,2,3;AUTOSAR:1,2,3" \
        -t fullhtml log.pvs -o pvsreport 2>&1 | tee /dev/stderr | \
          grep -Eq '(Exception: No valid messages|No messages generated)' && \
            { mkdir -p ./pvsreport;
                echo OK;
              printf '%s\n' "Congratulations!" > ./pvsreport/index.html;
            } || true
      mkdir -p ./pvsreport || true
      touch ./pvsreport/index.html
      grep -Eq '(Congratulations!|No messages generated)' ./pvsreport/index.html \
        || {
          printf '%s\n' "ERROR: PVS-Studio failed ..."
          printf '\n%s\n' "Review output in ./pvsreport ..."
          exit 99
        }
      rm -f ./compile_commands.json
      rm -f ./log.pvs
      rm -rf ./pvsreport
      printf '%s\n' "PVS-Studio lint completed."
      exit 0
) }

################################################################################

test_valgrind()
{ (
  command -v valgrind > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: valgrind not found, skipping checks."
      exit 1
    }
  command -v c++ > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: c++ not found, skipping valgrind checks."
      exit 1
    }
  command -v cc > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: cc not found, skipping valgrind checks."
      exit 1
    }
      printf '%s\n' "running valgrind checks ..."
      ${MAKE:-make} clean; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      env CXX="${CCACHE:-env} c++" \
           CC="${CCACHE:-env} cc" \
               "${MAKE:-make}" all tests++ \
                   -j "${CPUS:-1}" \
                   SIR_DEBUG=1 \
                   SIR_SELFLOG=1; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=98 \
          build/bin/sirexample; ret="${?}"
        test "${ret}" -eq 98 && exit 99
        exit 0
      ); ret="${?}"
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=98 \
          build/bin/sirtests; ret="${?}"
        test "${ret}" -eq 98 && exit 99
        exit 0
      ); ret="${?}"
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=98 \
          build/bin/sirtests++; ret="${?}"
        test "${ret}" -eq 98 && exit 99
        exit 0
      ); ret="${?}"
      test "${ret}" -ne 0 && exit 99
      exit 0
) }

################################################################################

test_reuse()
{ (
  command -v reuse > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: reuse not found, skipping checks."
      exit 1
    }
      printf '%s\n' "running reuse checks ..."
      reuse lint; ret="${?}"
      test "${ret}" -ne 0 && exit 99
      exit 0
) }

################################################################################

test_smoke0()
{ (
  exit 0
) }

################################################################################

test_smoke()
{ (
  test_smoke0; printf '%s\n' "${?}"
  exit 99
) }

################################################################################

runtest()
{
  "${@}"; ret="${?}"
  test "${ret}" -eq 99 && exit 99
  printf '%s\n' "Tool ${*} returned ${ret:-}"
  exit 0
}

################################################################################

test "${#}" -lt 1 2> /dev/null \
  || {
    printf '%s\n' "running only test_${1:?} ..."
    runtest "test_${1:?}"
    printf '%s\n' "End of test_${1:?} linting"
    exit 0
  }

################################################################################

(runtest test_spaces)     || exit 1
(runtest test_tabs)       || exit 1
(runtest whitespace)      || exit 1
(runtest test_duma)       || exit 1
(runtest test_extra)      || exit 1
(runtest test_flawfinder) || exit 1
(runtest test_cppcheck)   || exit 1
(runtest test_valgrind)   || exit 1
(runtest test_pvs)        || exit 1
(runtest test_reuse)      || exit 1

printf '%s\n' "End of linting"

################################################################################
