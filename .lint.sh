#!/usr/bin/env sh

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

# Variables:
#   VERBOSE - extra output is printed when set
#   DEBUGLN - debug output is printed when set
#   SKIPERR - makes many tool errors non-fatal

# Future plans:
#   Detect installed compilers and build with each
#   Run Oracle Lint

################################################################################

set -eu > /dev/null 2>&1

################################################################################

# shellcheck disable=SC2015
test -z "${DEBUGLN:-}" &&
  {
    set +x 2> /dev/null;
  } || {
    set -x 2> /dev/null;
    VERBOSE=1
  };

################################################################################

# shellcheck disable=SC2015
test -z "${VERBOSE:-}" &&
  {
    DEBUG_CALL="true";
  } || {
    DEBUG_CALL="echo";
  };

################################################################################

command -v "${GIT:-git}" > /dev/null 2>&1 ||
  {
    printf '%s\n' "ERROR: Unable to find git command!"
    exit "${EXIT_STATUS:-1}";
  }

################################################################################

CPUS="$(getconf _NPROCESSORS_ONLN 2> /dev/null || printf '%s\n' "1")"
${DEBUG_CALL:?} linting using "${CPUS:?}" processor\(s\) ...

################################################################################

# shellcheck disable=SC2015
test -z "${SKIPERR:-}" &&
  {
    EXIT_STATUS=1;
  } || {
    EXIT_STATUS=0;
  };

################################################################################

${DEBUG_CALL:?} checking for git toplevel ...
cd "$(${GIT:-git} rev-parse --show-toplevel)" ||
  {
    printf '%s\n' "ERROR: Unable to find git toplevel!"
    exit "${EXIT_STATUS:-1}";
  };

################################################################################

${DEBUG_CALL:?} checking for spaces in filenames ...
if (find src include -print | awk '{ print "\""$0"\"" }' | grep ' '); then
  {
    printf '%s\n' "ERROR: Filename check failed due to spaces!";
    exit "${EXIT_STATUS:-1}";
  };
fi

################################################################################

${DEBUG_CALL:?} checking source code for tabs ...
set +e; TLIST="$(find src include -print | grep '\.[ch]$' | \
    xargs -L 1 grep -l "$(printf '\t')" 2> /dev/null)"; set -e
printf "%s\n" "${TLIST:-}" | grep -v '^$' 2> /dev/null | grep . &&
  {
    printf '%s\n' "ERROR: Tabs check failed!";
    exit "${EXIT_STATUS:-1}";
  }

################################################################################

${DEBUG_CALL:?} checking source code for trailing whitespace ...
set +e; TLIST="$(find src include -print | \
    xargs -I{} grep -al ' \+$' "{}" 2> /dev/null)"; set -e
printf "%s\n" "${TLIST:-}" | grep -v '^$' 2> /dev/null | grep . &&
  {
    printf '%s\n' "ERROR: Trailing whitespace check failed!";
    exit "${EXIT_STATUS:-1}";
  }

################################################################################

${DEBUG_CALL:?} checking for "${MCMB:-build/bin/mcmb}" executable ...
test -x "${MCMB:-build/bin/mcmb}" ||
  {
    printf '%s\n' \
        "NOTICE: ${MCMB:-build/bin/mcmb} is not an executable, aborting ${0}."
    exit 1;
  }
SIR_OPTIONS="''                      \
             SIR_DEBUG=1             \
             SIR_NO_SYSTEM_LOGGERS=1 \
             SIR_NO_PLUGINS=1        \
             SIR_SELFLOG=1"

################################################################################

DUMA_OS="$(uname -s 2> /dev/null)"
test "${DUMA_OS:-}" = "Darwin" &&
  {
    command -v /opt/duma/bin/duma > /dev/null 2>&1 ||
      {
        printf '%s\n' \
            "NOTICE: DUMA not found, skipping DUMA check."
        NO_DUMA=1
      }
  }
test "${DUMA_OS:-}" = "Darwin" ||
  {
    command -v duma > /dev/null 2>&1 ||
      {
        printf '%s\n' \
            "NOTICE: DUMA not found, skipping DUMA check."
        NO_DUMA=1
      }
  }
command -v gcc > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: GCC is required for DUMA check."
    NO_DUMA=1
  }
test -z "${NO_DUMA:-}" &&
  {
    rm -f ./duma*.log
    ${DEBUG_CALL:?} building with DUMA ...
    env ${MAKE:-make} clean
    env CC="gcc"                                  \
        EXTRA_LIBS="-L/opt/duma/lib -l:libduma.a" \
        CFLAGS="-DDUMA=1"                         \
            ${MAKE:-make}                         \
                -j "${CPUS:-1}"                   \
                SIR_DEBUG=1                       \
                SIR_SELFLOG=1
    ${DEBUG_CALL:?} running DUMA-enabled example ...
    env DUMA_OUTPUT_FILE=duma1.log \
        DUMA_OUTPUT_STDERR=0       \
        DUMA_OUTPUT_STDOUT=0       \
            build/bin/sirexample
    env DUMA_OUTPUT_FILE=duma2.log \
        DUMA_PROTECT_BELOW=1       \
        DUMA_OUTPUT_STDERR=0       \
        DUMA_OUTPUT_STDOUT=0       \
            build/bin/sirexample
    ${DEBUG_CALL:?} running DUMA-enabled tests ...
    env DUMA_OUTPUT_FILE=duma3.log \
        DUMA_OUTPUT_STDERR=0       \
        DUMA_OUTPUT_STDOUT=0       \
            build/bin/sirtests
    env DUMA_OUTPUT_FILE=duma4.log \
        DUMA_PROTECT_BELOW=1       \
        DUMA_OUTPUT_STDERR=0       \
        DUMA_OUTPUT_STDOUT=0       \
            build/bin/sirtests
    sleep 1 || true
    DUMAOUT="$(cat ./duma*.log 2> /dev/null |                                 \
                grep DUMA 2> /dev/null |                                      \
                grep -Ev 2> /dev/null                                         \
                '(extra leak|Reported |compiled| alloced from.*duma\.c)')" || \
                    true
    test -z "${DUMAOUT:-}" ||
      {
        printf '%s\n' "ERROR: DUMA failed, output follows ..."
        printf '%s\n' "${DUMAOUT:?}"
        exit "${EXIT_STATUS:-1}";
      }
    rm -f ./duma*.log
  }

################################################################################

command -v clang > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: clang is required for the extra-warning check."
    NO_EXTRAWARN=1
  }
test -z "${NO_EXTRAWARN:-}" &&
  {
    ${DEBUG_CALL:?} building with extra-warning flags ...
    env ${MAKE:-make} clean
    rm -f ./.extra.sh
    env CC="clang"              \
            ${MAKE:-make}       \
                -j "${CPUS:-1}" \
                mcmb
    printf '%s' 'true' > ./.extra.sh
    ${MCMB:-build/bin/mcmb} -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
        env CC="clang"
            CFLAGS="-Werror
                    -Wmissing-prototypes
                    -Wdouble-promotion
                    -Wconversion
                    -Wno-sign-conversion
                    -Wno-string-conversion"
            ${MAKE:-make}
                -j "${CPUS:-1}" ' | tr '\n' ' ' | tr -s ' ' >> ./.extra.sh
    printf '%s\n' ' && true' >> ./.extra.sh
    chmod a+x ./.extra.sh
    sh ./.extra.sh
    rm -f ./.extra.sh
  }

################################################################################

command -v flawfinder > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: flawfinder not found, skipping check."
    NO_FLAWFINDER=1
  }
test -z "${NO_FLAWFINDER:-}" &&
  {
    ${DEBUG_CALL:?} running flawfinder check ...
    FLAWFINDER_OUTPUT="$(flawfinder -C -m 5 -c . 2>&1)" || true
    printf '%s\n' "${FLAWFINDER_OUTPUT:-}" | grep -q "No hits found" ||
      {
        printf '%s\n' "ERROR: Flawfinder failed, output follows ..."
        printf '%s\n' "${FLAWFINDER_OUTPUT:-}"
        exit "${EXIT_STATUS:-1}";
      }
  }

################################################################################

command -v scan-build > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: scan-build not found, skipping check."
    NO_SCANBUILD=1
  }
test -z "${NO_SCANBUILD:-}" &&
  {
    ${DEBUG_CALL:?} running scan-build check ...
    env ${MAKE:-make} clean
    env CC="clang" ${MAKE:-make} \
                -j "${CPUS:-1}"  \
                mcmb
    printf '%s' 'true' > ./.scan-build.sh
    ${MCMB:-build/bin/mcmb} -e ${SIR_OPTIONS:?} | xargs -L1 echo \
        ' && ${MAKE:-make} clean &&
         env CC="clang"
           scan-build -no-failure-reports
               --status-bugs
               -maxloop 700
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

################################################################################

command -v cppcheck > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: cppcheck not found, skipping check."
    NO_CPPCHECK=1
  }
command -v cppcheck-htmlreport > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: cppcheck-htmlreport not found, skipping check."
    NO_CPPCHECK=1
  }
test -z "${NO_CPPCHECK:-}" &&
  {
    ${DEBUG_CALL:?} running cppcheck check ...
    ${MAKE:-make} clean
    rm -rf ./cppcheck
    rm -f ./cppcheck.xml
    mkdir -p cppcheck
    cppcheck --force                                      \
             --enable="warning,performance,portability"   \
             --suppress=shadowArgument                    \
             --suppress=shadowVariable                    \
             --suppress=shadowFunction                    \
             --suppress=ConfigurationNotChecked           \
             --suppress=unknownMacro                      \
             --suppress=syntaxError:/usr/include/stdlib.h \
             --suppress=unmatchedSuppression              \
             --suppress=variableScope                     \
             --suppress=va_list_usedBeforeStarted         \
             --suppress="*:/usr/*"                        \
             --inline-suppr                               \
             --max-ctu-depth="16"                         \
             --platform=unix64                            \
             --std="c11"                                  \
             --inconclusive                               \
             -j ${CPUS:?}                                 \
                  $(find . -name '*.[ch]' |               \
                      grep -v 'mcmb.c')                   \
                   --xml --xml-version=2 2> cppcheck.xml  \
    && cppcheck-htmlreport --source-dir="."               \
                           --report-dir="./cppcheck"      \
                           --file="cppcheck.xml"
    grep -q '<td>0</td><td>total</td>' ./cppcheck/index.html ||
      {
        printf '%s\n' "ERROR: Cppcheck failed ..."
        exit "${EXIT_STATUS:-1}";
      }
    rm -rf ./cppcheck
    rm -f ./cppcheck.xml
  }

################################################################################

command -v clang > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: clang not found, skipping PVS-Studio checks."
    NO_PVSSTUDIO=1
  }
command -v bear > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: bear not found, skipping PVS-Studio checks."
    NO_PVSSTUDIO=1
  }
command -v pvs-studio-analyzer > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: pvs-studio-analyzer not found, skipping PVS-Studio check."
    NO_PVSSTUDIO=1
  }
command -v plog-converter > /dev/null 2>&1 ||
  {
    printf '%s\n' \
        "NOTICE: plog-converter not found, skipping PVS-Studio checks."
    NO_PVSSTUDIO=1
  }
test -z "${NO_PVSSTUDIO:-}" &&
  {
    ${DEBUG_CALL:?} running PVS-Studio checks ...
    rm -rf ./pvsreport
    rm -f ./log.pvs
    rm -f ./compile_commands.json
    ${MAKE:-make} clean
    env CC="clang" bear -- ${MAKE:-make} -j "${CPUS:-1}"
    pvs-studio-analyzer analyze --intermodular -j "${CPUS:-1}" -o log.pvs
    plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -o pvsreport
    grep -q 'Congratulations!' ./cppcheck/index.html ||
      {
        printf '%s\n' "ERROR: PVS-Studio failed ..."
        exit "${EXIT_STATUS:-1}";
      }
    rm -f ./compile_commands.json
    rm -f ./log.pvs
    rm -f ./pvsreport
  }

################################################################################
