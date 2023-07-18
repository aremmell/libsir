#!/bin/sh

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

# Setup for Ubuntu or Debian.
PATH="/usr/local/bin:/usr/local/sbin:${PATH:-}" && export PATH
test -n "${NO_APTSETUP:-}" \
  || {
    { # Check this is Ubuntu 22.04 Jammy
      grep '22.04' /etc/*elease || exit 1
      printf '\n\n\n%s\n\n\n' "About to run commands as root - press ^C now if you didn't read the script!!!" ;
      sleep 6 ;
      export DEBIAN_FRONTEND=noninteractive ;
      sudo apt-get update -y ;
      sudo apt-get -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" install -y ccache curl python3-pip git ;
      #sudo python3 -m pip install --break-system-packages install -U gcovr || true ;  # for later Ubuntu versions
      sudo python3 -m pip install install -U gcovr || true ;
      command -v gcovr || true ;
    }
  }

# NOTE: Needs GNU rm

set -ex

# (Try to) disable messages to terminal from syslog
mesg n || true

# How parallel? Use "1" for Travis CI.
JOBS=1

# What MAKE to use?
test -n "${MAKE:-}" && DO_MAKE="${MAKE:-}"

# Clean-up
cleanup_files()
{
  rm -f ./coveralls > /dev/null 2>&1
  rm -f ./coverage-out*.* > /dev/null 2>&1
  rm -f ./coveralls.json > /dev/null 2>&1
}

cleanup_files

# Make sure we have a token set.
test -n "${NO_COVERALLS:-}" \
  || {
    test -n "${COVERALLS_REPO_TOKEN:-}" \
      || {
        printf '%s\n' "Error: COVERALLS_REPO_TOKEN is unset."
        exit 1
      }
  }

# Test for command
test_for()
{
  command -v "${1:-}" > /dev/null 2>&1 \
    || {
      printf '%s\n' "Error: ${1:-} not found."
      exit 1
    }
}

# Sanity check.
test_for curl
test_for gcovr
test_for git

# Get coveralls tool.
curl -fsSL https://coveralls.io/coveralls-linux.tar.gz | tar -xz
chmod a+x ./coveralls

# Setup compiler.
CC="ccache gcc"
CFLAGS="-fno-inline -fprofile-arcs -ftest-coverage --coverage"
LDFLAGS="-fprofile-arcs -ftest-coverage"
export CC CFLAGS LDFLAGS

# Removes sample plugin results
remove_sample()
{
  # shellcheck disable=SC2046
  env rm -v $(find . -name 'plugin_sample*.*gc*' -print) || true
}

# Removes all coverage files
remove_coverage()
{
  # shellcheck disable=SC2046
  env rm -v $(find . -name '*.*gc*' -print) || true
}

# Runs gcovr
run_gcovr()
{
  gcovr --gcov-ignore-parse-errors=negative_hits.warn_once_per_file --json "${1}"
}

# Redirect
exec 5>&1 > coverage-out.txt 2>&1

# Run 1 - Debug and self-log
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample
build/bin/sirtests
remove_sample || true
run_gcovr run-1.json
remove_coverage

# Run 2 - No plugins
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests
remove_sample || true
run_gcovr run-2.json
remove_coverage

# Run 3 - Perf test
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --perf
remove_sample || true
run_gcovr run-3.json
remove_coverage

# Run 4 - Test help
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --help
remove_sample || true
run_gcovr run-4.json
remove_coverage

# Run 5 - List tests
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --list
remove_sample || true
run_gcovr run-5.json
remove_coverage

# Run 6 - Version check
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --version
remove_sample || true
run_gcovr run-6.json
remove_coverage

# Run 7 - Invalid arguments to sirtest
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --INVALID_ARGUMENTS || true
remove_sample || true
run_gcovr run-7.json
remove_coverage

# Run 8 - Bad `--only` without arguments to sirtest
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --only || true
remove_sample || true
run_gcovr run-8.json
remove_coverage

# Run 9 - No debug and no self-log
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?}
build/bin/sirexample
build/bin/sirtests
remove_sample || true
run_gcovr run-9.json
remove_coverage

# Run 10 - Just self-log
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_SELFLOG=1
build/bin/sirexample
build/bin/sirtests
remove_sample || true
run_gcovr run-10.json
remove_coverage

# Undo redirect
exec 1>&5

# Process results
MERGE_MODE="merge-use-line-0"
gcovr \
  --add-tracefile "run-*.json" \
  --merge-mode-functions="${MERGE_MODE:?}" \
  --gcov-ignore-parse-errors=negative_hits.warn_once_per_file --html-details coverage-out.html
gcovr \
  --add-tracefile "run-*.json" \
  --merge-mode-functions="${MERGE_MODE:?}" \
  --gcov-ignore-parse-errors=negative_hits.warn_once_per_file --coveralls coveralls.json

# Submit results
test -n "${NO_COVERALLS:-}" || ./coveralls coveralls.json -r "${COVERALLS_REPO_TOKEN:?}"

# Cleanup
test -n "${NO_CLEANUP:-}" \
  || {
    cleanup_files
    rm -f ./run*.json > /dev/null 2>&1
  }
