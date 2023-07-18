#!/bin/sh

# Setup for Ubuntu or Debian.
test -n "${NO_APTSETUP:-}" \
  || {
    ( export DEBIAN_FRONTEND=noninteractive ;
      sudo apt-get update -y ;
      sudo apt-get -o Dpkg::Options::="--force-confdef" -o Dpkg::Options::="--force-confold" install -y curl gcovr git ;
      true
    )
  }

# NOTE: Needs GNU rm

set -ex

# How parallel?
JOBS=1

# What make to use?
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
test -n "${COVERALLS_REPO_TOKEN:-}" \
  || {
    printf '%s\n' "Error: COVERALLS_REPO_TOKEN is unset."
    exit 1
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
CC="gcc"
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

# Run 1
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample
build/bin/sirtests
remove_sample || true
run_gcovr run-1.json
remove_coverage

# Run 2
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests
remove_sample || true
run_gcovr run-2.json
remove_coverage

# Run 3
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --perf
remove_sample || true
run_gcovr run-3.json
remove_coverage

# Run 4
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --help
remove_sample || true
run_gcovr run-4.json
remove_coverage

# Run 5
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --list
remove_sample || true
run_gcovr run-5.json
remove_coverage

# Run 6
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample
build/bin/sirtests --version
remove_sample || true
run_gcovr run-6.json
remove_coverage

# Process results
MERGE_MODE="merge-use-line-max"
gcovr \
  --add-tracefile run-1.json \
  --add-tracefile run-2.json \
  --add-tracefile run-3.json \
  --add-tracefile run-4.json \
  --add-tracefile run-5.json \
  --add-tracefile run-6.json \
  --merge-mode-functions="${MERGE_MODE:?}" \
  --gcov-ignore-parse-errors=negative_hits.warn_once_per_file --html-details coverage-out.html
gcovr \
  --add-tracefile run-1.json \
  --add-tracefile run-2.json \
  --add-tracefile run-3.json \
  --add-tracefile run-4.json \
  --add-tracefile run-5.json \
  --add-tracefile run-6.json \
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
