#!/usr/bin/env sh

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

################################################################################

set -eu > /dev/null 2>&1

################################################################################

command -v "${GIT:-git}" > /dev/null 2>&1 \
  || {
    printf '%s\n' "ERROR: Unable to find git command!"
    exit 1
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
    exit 1
  }

################################################################################

test_spaces()
{
  printf '%s\n' "checking for spaces in filenames ..."
  if (find src include -print | awk '{ print "\""$0"\"" }' | grep ' '); then
    {
      printf '%s\n' "ERROR: Filename check failed due to spaces!"
      exit 1
    }
  fi
}

################################################################################

test_tabs()
{
  printf '%s\n' "checking source code for tabs ..."
  TLIST="$(find src include -print | grep '\.[ch]$' \
    | xargs -L 1 grep -l "$(printf '\t')" 2> /dev/null)" || true
  # shellcheck disable=SC2015
  printf "%s\n" "${TLIST:-}" | grep -v '^$' 2> /dev/null | grep . \
    && {
      printf '%s\n' "ERROR: Tabs check failed!"
      exit 1
    } || true
}

################################################################################

test_whitespace()
{
  printf '%s\n' "checking source code for trailing whitespace ..."
  # shellcheck disable=SC2038
  TLIST="$(find src include -print \
    | xargs -I{} grep -al ' \+$' "{}" 2> /dev/null)" || true
  # shellcheck disable=SC2015
  printf "%s\n" "${TLIST:-}" | grep -v '^$' 2> /dev/null | grep . \
    && {
      printf '%s\n' "ERROR: Trailing whitespace check failed!"
      exit 1
    } || true
}

################################################################################

test_mcmb()
{
  ${MAKE:-make} mcmb
  printf '%s\n' "checking for ${MCMB:-build/bin/mcmb} executable ..."
  test -x "${MCMB:-build/bin/mcmb}" \
    || {
      printf '%s\n' \
        "NOTICE: ${MCMB:-build/bin/mcmb} is not an executable, aborting ${0}."
      exit 1
    }
  # shellcheck disable=SC2089
  SIR_OPTIONS="''                      \
             SIR_DEBUG=1             \
             SIR_NO_SYSTEM_LOGGERS=1 \
             SIR_NO_PLUGINS=1        \
             SIR_SELFLOG=1"
  # shellcheck disable=SC2090
  export SIR_OPTIONS
}

################################################################################

test_duma()
{
  DUMA_OS="$(uname -s 2> /dev/null)"
  test "${DUMA_OS:-}" = "Darwin" \
    && {
      printf '%s\n' \
        "NOTICE: Skipping DUMA checks on macOS."
      NO_DUMA=1
    }
  test "${DUMA_OS:-}" = "Darwin" \
    || {
      command -v duma > /dev/null 2>&1 \
        || {
          printf '%s\n' \
            "NOTICE: DUMA not found, skipping DUMA check."
          NO_DUMA=1
        }
    }
  command -v gcc > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: GCC is required for DUMA check."
      NO_DUMA=1
    }
  test -z "${NO_DUMA:-}" \
    && {
      rm -f ./duma*.log
      printf '%s\n' "building with DUMA ..."
      env "${MAKE:-make}" clean
      env CC="${CCACHE:-env} gcc" \
        EXTRA_LIBS="-L/opt/duma/lib -l:libduma.a" \
        CFLAGS="-I/opt/duma/include -DDUMA=1" \
        "${MAKE:-make}" \
        -j "${CPUS:-1}" \
        SIR_DEBUG=1 \
        SIR_SELFLOG=1
      printf '%s\n' "running DUMA-enabled example ..."
      env DUMA_OUTPUT_FILE=duma1.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirexample
      env DUMA_OUTPUT_FILE=duma2.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirexample
      printf '%s\n' "running DUMA-enabled tests ..."
      env DUMA_OUTPUT_FILE=duma3.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests
      env DUMA_OUTPUT_FILE=duma4.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests
      sleep 1 || true
      DUMAOUT="$(cat ./duma*.log 2> /dev/null \
        | grep DUMA 2> /dev/null \
        | grep -Ev \
          '(extra leak|Reported |compiled| alloced from.*duma\.c)' 2> /dev/null
      )" \
        || true
      test -z "${DUMAOUT:-}" \
        || {
          printf '%s\n' "ERROR: DUMA failed, output follows ..."
          printf '%s\n' "${DUMAOUT:?}"
          exit 1
        }
      rm -f ./duma*.log
    }
}

################################################################################

test_extra()
{
  test_mcmb
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang is required for the extra-warning check."
      NO_EXTRAWARN=1
    }
  test -z "${NO_EXTRAWARN:-}" \
    && {
      printf '%s\n' "building with extra-warning flags ..."
      env "${MAKE:-make}" clean
      rm -f ./.extra.sh
      env CC="${CCACHE:-env} clang" \
        "${MAKE:-make}" \
        -j "${CPUS:-1}" \
        mcmb
      printf '%s' 'true' > ./.extra.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
        env CC="${CCACHE:-env} clang"
            CFLAGS="-Werror
                    -Wmissing-prototypes
                    -Wdouble-promotion
                    -Wmissing-variable-declarations
                    -Wconversion
                    -Wbad-function-cast
                    -Wno-sign-conversion
                    -Wswitch-enum
                    -Wno-string-conversion"
            ${MAKE:-make}
                -j "${CPUS:-1}" ' | tr '\n' ' ' | tr -s ' ' >> ./.extra.sh
      printf '%s\n' ' && true' >> ./.extra.sh
      chmod a+x ./.extra.sh
      sh ./.extra.sh
      rm -f ./.extra.sh
    }
}

################################################################################

test_flawfinder()
{
  command -v flawfinder > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: flawfinder not found, skipping check."
      NO_FLAWFINDER=1
    }
  test -z "${NO_FLAWFINDER:-}" \
    && {
      printf '%s\n' "running flawfinder check ..."
      FLAWFINDER_OUTPUT="$(flawfinder -C -m 5 -c . 2>&1)" || true
      printf '%s\n' "${FLAWFINDER_OUTPUT:-}" | grep -q "No hits found" \
        || {
          printf '%s\n' "ERROR: Flawfinder failed, output follows ..."
          printf '%s\n' "${FLAWFINDER_OUTPUT:-}"
          exit 1
        }
    }
}

################################################################################

test_scanbuild()
{
  test_mcmb
  command -v scan-build > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: scan-build not found, skipping check."
      NO_SCANBUILD=1
    }
  test -z "${NO_SCANBUILD:-}" \
    && {
      printf '%s\n' "running scan-build check ..."
      env "${MAKE:-make}" clean
      env CC="${CCACHE:-env} clang" "${MAKE:-make}" \
        -j "${CPUS:-1}" \
        mcmb
      printf '%s' 'true' > ./.scan-build.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean && ${MAKE:-make} mcmb &&
         env CC="${CCACHE:-env} clang"
           scan-build -no-failure-reports
               --status-bugs
               -maxloop 8
               -enable-checker optin.portability.UnixAPI
               -enable-checker security.FloatLoopCounter
               -enable-checker security.insecureAPI.bcmp
               -enable-checker security.insecureAPI.bcopy
                   ${MAKE:-make}
                       -j "${CPUS:-1}" ' \
        | tr '\n' ' ' | tr -s ' ' >> ./.scan-build.sh
      printf '%s\n' ' && true' >> ./.scan-build.sh
      chmod a+x ./.scan-build.sh
      sh ./.scan-build.sh
      rm -f ./.scan-build.sh
      rm -rf ./clang-analyzer 2> /dev/null
    }
}

################################################################################

test_cppcheck()
{
  command -v cppcheck > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: cppcheck not found, skipping check."
      NO_CPPCHECK=1
    }
  command -v cppcheck-htmlreport > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: cppcheck-htmlreport not found, skipping check."
      NO_CPPCHECK=1
    }
  test -z "${NO_CPPCHECK:-}" \
    && {
      printf '%s\n' "running cppcheck check ..."
      ${MAKE:-make} clean
      rm -rf ./cppcheck
      rm -f ./cppcheck.xml
      mkdir -p cppcheck
      # shellcheck disable=SC2046
      cppcheck --force \
        --enable="warning,performance,portability" \
        --suppress=ConfigurationNotChecked \
        --suppress=unknownMacro \
        --suppress=syntaxError:/usr/include/stdlib.h \
        --suppress=unmatchedSuppression \
        --suppress=va_list_usedBeforeStarted \
        --suppress="*:/usr/*" \
        --inline-suppr \
        --max-ctu-depth="16" \
        --platform=unix64 \
        --std="c11" \
        --inconclusive \
        -j "${CPUS:?}" \
        $(find . -name '*.[ch]' \
          | grep -v 'mcmb.c') \
        --xml --xml-version=2 2> cppcheck.xml \
        && cppcheck-htmlreport --source-dir="." \
          --report-dir="./cppcheck" \
          --file="cppcheck.xml"
      grep -q '<td>0</td><td>total</td>' ./cppcheck/index.html \
        || {
          printf '%s\n' "ERROR: Cppcheck failed ..."
          exit 1
        }
      rm -rf ./cppcheck
      rm -f ./cppcheck.xml
    }
}

################################################################################

test_pvs()
{
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang not found, skipping PVS-Studio checks."
      NO_PVSSTUDIO=1
    }
  command -v bear > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: bear not found, skipping PVS-Studio checks."
      NO_PVSSTUDIO=1
    }
  command -v pvs-studio-analyzer > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: pvs-studio-analyzer not found, skipping PVS-Studio check."
      NO_PVSSTUDIO=1
    }
  command -v plog-converter > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: plog-converter not found, skipping PVS-Studio checks."
      NO_PVSSTUDIO=1
    }
  test -z "${NO_PVSSTUDIO:-}" \
    && {
      printf '%s\n' "running PVS-Studio checks ..."
      rm -rf ./pvsreport
      rm -f ./log.pvs
      rm -f ./compile_commands.json
      ${MAKE:-make} clean
      ${MAKE:-make} mcmb
      env CC="${CCACHE:-env} clang" bear -- "${MAKE:-make}" -j "${CPUS:-1}"
      pvs-studio-analyzer analyze --intermodular -j "${CPUS:-1}" -o log.pvs
      plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -o pvsreport
      grep -q 'Congratulations!' ./pvsreport/index.html \
        || {
          printf '%s\n' "ERROR: PVS-Studio failed ..."
          printf '\n%s\n' "Review output in ./pvsreport ..."
          exit 1
        }
      rm -f ./compile_commands.json
      rm -f ./log.pvs
      rm -rf ./pvsreport
    }
}

################################################################################

test_valgrind()
{
  command -v valgrind > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: valgrind not found, skipping checks."
      NO_VALGRIND=1
    }
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang not found, skipping valgrind checks."
      NO_VALGRIND=1
    }
  test -z "${NO_VALGRIND:-}" \
    && {
      printf '%s\n' "running valgrind checks ..."
      ${MAKE:-make} clean
      env CC="${CCACHE:-env} clang" "${MAKE:-make}" -j "${CPUS:-1}" SIR_DEBUG=1 SIR_SELFLOG=1
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=1 \
          build/bin/sirexample
      )
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=1 \
          build/bin/sirtests
      )
    }
}

################################################################################

test_reuse()
{
  command -v reuse > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: reuse not found, skipping checks."
      NO_REUSE=1
    }
  test -z "${NO_REUSE:-}" \
    && {
      printf '%s\n' "running reuse checks ..."
      reuse lint
    }
}

################################################################################

test_smoke()
{
  exit 0
}

################################################################################

test_spaces

################################################################################

test "${#}" -lt 1 2> /dev/null \
  || {
    printf '%s\n' "running only test_${1:?} ..."
    "test_${1:?}"
    sleep 1 || true
    printf '%s\n' "End of test_${1:?} linting"
    exit 0
  }

################################################################################

test_duma
test_extra
test_flawfinder
test_cppcheck
test_pvs
test_valgrind
test_reuse

sleep 1 || true
printf '%s\n' "End of linting"

################################################################################
