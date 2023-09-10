#!/usr/bin/env sh
# shellcheck disable=SC2312,SC2317
# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

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
  sleep 2
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
  sleep 2
  TLIST="$(find src include -print | grep '\.[ch]$' \
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
  sleep 2
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
  ${MAKE:-make} mcmb; ret=${?}
  test "${ret}" -ne 0 && exit 99
  printf '%s\n' "checking for ${MCMB:-build/bin/mcmb} executable ..."
  sleep 2
  test -x "${MCMB:-build/bin/mcmb}" \
    || {
      printf '%s\n' \
        "NOTICE: ${MCMB:-build/bin/mcmb} is not an executable, aborting ${0}."
      exit 99
    }
  # shellcheck disable=SC2089
  SIR_OPTIONS="'' \
             SIR_DEBUG=1 \
             SIR_NO_SYSTEM_LOGGERS=1 \
             SIR_NO_PLUGINS=1 \
             SIR_DPSPRINTF=1 \
             SIR_SELFLOG=1"
  # shellcheck disable=SC2090
  printf '%s\n' "exported SIR_OPTIONS=' ${SIR_OPTIONS} '" | tr -s ' '
  # shellcheck disable=SC2090
  export SIR_OPTIONS
}

################################################################################

test_duma()
{ (
  printf '%s\n' "checking for duma ..."
  sleep 2
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
  command -v gcc > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: GCC is required for DUMA check."
      exit 1
    }
      rm -f ./duma*.log
      printf '%s\n' "building with DUMA ..."
      sleep 1
      env "${MAKE:-make}" clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      env DUMA=1 \
          CC="gcc" \
          EXTRA_LIBS="-L/opt/duma/lib" \
          CFLAGS="-I/opt/duma/include" \
        "${MAKE:-make}" \
          -j 1 \
        SIR_DEBUG=1 \
        SIR_SELFLOG=1; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "running DUMA-enabled example ..."
      sleep 1
      env DUMA_OUTPUT_FILE=duma1.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirexample; ret=${?}
      test "${ret}" -ne 0 && exit 99
      env DUMA_OUTPUT_FILE=duma2.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirexample; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "running DUMA-enabled tests ..."
      sleep 1
      env DUMA_OUTPUT_FILE=duma3.log \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests; ret=${?}
      test "${ret}" -ne 0 && exit 99
      env DUMA_OUTPUT_FILE=duma4.log \
        DUMA_PROTECT_BELOW=1 \
        DUMA_OUTPUT_STDERR=0 \
        DUMA_OUTPUT_STDOUT=0 \
        build/bin/sirtests; ret=${?}
      test "${ret}" -ne 0 && exit 99
      sleep 1
      printf '%s\n' "checking DUMA output ..."
      sleep 1
      test -f duma1.log || DUMA_FAIL=1
      test -f duma2.log || DUMA_FAIL=1
      test -f duma3.log || DUMA_FAIL=1
      test -f duma4.log || DUMA_FAIL=1
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
  test_mcmb; ret=${?}
  test "${ret}" -ne 0 && exit 99
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang is required for the extra-warning check."
      exit 1
    }
      printf '%s\n' "building with extra-warning flags ..."
      sleep 2
      env "${MAKE:-make}" clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh
      env CC="${CCACHE:-env} clang" \
        "${MAKE:-make}" \
        -j "${CPUS:-1}" \
        mcmb; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s' 'true' > ./.extra.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
        env CC="${CCACHE:-env} clang"
            CFLAGS="-Werror
                    -Wassign-enum
                    -Wbad-function-cast
                    -Wconversion
                    -DSIR_LINT=1
                    -Wdisabled-macro-expansion
                    -Wdouble-promotion
                    -Wextra-semi-stmt
                    -Wformat=2
                    -Wformat-nonliteral
                    -Winvalid-utf8
                    -Wmissing-prototypes
                    -Wmissing-variable-declarations
                    -Wno-sign-conversion
                    -Wno-string-conversion
                    -Wshift-overflow
                    -Wstring-conversion
                    -Wswitch-enum"
            ${MAKE:-make}
                -j "${CPUS:-1}" ' | tr '\n' ' ' | tr -s ' ' >> ./.extra.sh
      printf '%s\n' ' && true' >> ./.extra.sh; ret=${?}
      test "${ret}" -ne 0 && exit 99
      chmod a+x ./.extra.sh
      sh ./.extra.sh; ret=${?}
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh
      exit 0
) }

################################################################################

test_gccextra()
{ (
  test_mcmb; ret=${?}
  test "${ret}" -ne 0 && exit 99
  gcc --version 2>&1 | grep -qi GCC > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: gcc is required for the gcc extra-warning check."
      exit 1
    }
      printf '%s\n' "building with extra-warning flags ..."
      sleep 2
      env "${MAKE:-make}" clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh
      env CC="${CCACHE:-env} gcc" \
        "${MAKE:-make}" \
        -j 1 \
        mcmb; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s' 'true' > ./.extra.sh
      # shellcheck disable=SC2090,SC2086,SC2016
      "${MCMB:-build/bin/mcmb}" -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
        env CC="${CCACHE:-env} gcc"
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
                    -Wswitch-enum"
            ${MAKE:-make}
                -j 1 ' | tr '\n' ' ' | tr -s ' ' >> ./.extra.sh; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' ' && true' >> ./.extra.sh
      chmod a+x ./.extra.sh
      sh ./.extra.sh; ret=${?}
      test "${ret}" -ne 0 && exit 99
      rm -f ./.extra.sh
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
      sleep 2
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
      sleep 2
      env "${MAKE:-make}" clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      env CC="${CCACHE:-env} clang" "${MAKE:-make}" \
        -j "${CPUS:-1}" \
        mcmb; ret=${?}
      test "${ret}" -ne 0 && exit 99
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
        | tr '\n' ' ' | tr -s ' ' >> ./.scan-build.sh; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' ' && true' >> ./.scan-build.sh
      chmod a+x ./.scan-build.sh
      sh ./.scan-build.sh; ret=${?}
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
      sleep 2
      ${MAKE:-make} clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      rm -rf ./cppcheck
      rm -f ./cppcheck.xml
      mkdir -p cppcheck; ret=${?}
      test "${ret}" -ne 0 && exit 99
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
        -UDPS_SPRINTF_LD \
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
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang not found, skipping PVS-Studio checks."
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
      sleep 2
      rm -rf ./pvsreport
      rm -f ./log.pvs
      rm -f ./compile_commands.json
      ${MAKE:-make} clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      ${MAKE:-make} mcmb; ret=${?}
      test "${ret}" -ne 0 && exit 99
      env CC="${CCACHE:-env} clang" bear -- "${MAKE:-make}" SIR_DPSPRINTF=1 -j "${CPUS:-1}"; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "Running PVS-Studio ..."
      pvs-studio-analyzer analyze --intermodular -j "${CPUS:-1}" -o log.pvs
      test -f log.pvs; ret=${?}
      test "${ret}" -ne 0 && exit 99
      printf '%s\n' "PVS-Studio run completed ..."
      plog-converter -a "GA:1,2,3;OP:1,2,3;64:1,2,3;CS:1,2,3;MISRA:1,2,3;OWASP:1,2,3;AUTOSAR:1,2,3" -t fullhtml log.pvs -o pvsreport
      grep -q 'Congratulations!' ./pvsreport/index.html \
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
  command -v clang > /dev/null 2>&1 \
    || {
      printf '%s\n' \
        "NOTICE: clang not found, skipping valgrind checks."
      exit 1
    }
      printf '%s\n' "running valgrind checks ..."
      sleep 2
      ${MAKE:-make} clean; ret=${?}
      test "${ret}" -ne 0 && exit 99
      env CC="${CCACHE:-env} clang" "${MAKE:-make}" -j "${CPUS:-1}" SIR_DEBUG=1 SIR_SELFLOG=1; ret=${?}
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=98 \
          build/bin/sirexample; ret=${?}
        test "${ret}" -eq 98 && exit 99
        exit 0
      ); ret=${?}
      test "${ret}" -ne 0 && exit 99
      # shellcheck disable=SC3045
      (
        ulimit -n 384
        valgrind \
          --leak-check=full \
          --track-origins=yes \
          --error-exitcode=98 \
          build/bin/sirtests; ret=${?}
        test "${ret}" -eq 98 && exit 99
        exit 0
      ); ret=${?}
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
      sleep 2
      reuse lint; ret=${?}
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
  "${@}"; ret=${?}
  test "${ret}" -eq 99 && exit 99
  printf '%s\n' "Tool ${*} returned ${ret:-}"
  exit 0
}

################################################################################

test "${#}" -lt 1 2> /dev/null \
  || {
    printf '%s\n' "running only test_${1:?} ..."
    runtest "test_${1:?}"
    sleep 1
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
(runtest test_pvs)        || exit 1
(runtest test_valgrind)   || exit 1
(runtest test_reuse)      || exit 1

sleep 1
printf '%s\n' "End of linting"

################################################################################
