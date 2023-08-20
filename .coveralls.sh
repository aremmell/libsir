#!/bin/sh

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman

# Setup for Ubuntu or Debian unless NO_APTSETUP is set.
PATH="/usr/local/bin:/usr/local/sbin:${PATH:-}" && export PATH
test -n "${NO_APTSETUP:-}" \
  || {
       printf '\n\n\n%s\n\n\n' \
         "Running dangerous commands as root in 10s; press ^C now to abort."
       sleep 10
       export DEBIAN_FRONTEND=noninteractive
       sudo apt-get update -y
       sudo apt-get -o Dpkg::Options::="--force-confdef" \
                    -o Dpkg::Options::="--force-confold" \
            install -y ccache curl python3-pip git expect fakeroot
       sudo python3 -m pip install --break-system-packages \
            install -U gcovr || \
               sudo python3 -m pip install \
                    install -U gcovr
  }

set -ex

# How parallel? Use "1" for Travis CI.
JOBS=1

# What MAKE to use?
test -n "${MAKE:-}" && DO_MAKE="${MAKE:-}"

# Clean-up
cleanup_files()
{
  rm -f ./coverage-out*.* > /dev/null 2>&1
  rm -f ./coveralls.json  > /dev/null 2>&1
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

# Setup compiler.
CC="ccache gcc"
CFLAGS="-fno-inline -fprofile-arcs -ftest-coverage --coverage"
LDFLAGS="-fprofile-arcs -ftest-coverage"
export CC CFLAGS LDFLAGS

# Removes unwanted results
remove_sample()
{
  # shellcheck disable=SC2046
  env rm $(find . -name 'plugin_sample*.*gc*' -print) || true
  # shellcheck disable=SC2046
  env rm $(find . -name 'mcmb*.*gc*' -print) || true
}

# Removes all coverage files
remove_coverage()
{
  # shellcheck disable=SC2046
  env rm $(find . -name '*.*gc*' -print) || true
  sleep 1 || true
}

# Runs gcovr
run_gcovr()
{
  gcovr \
      --gcov-ignore-parse-errors=negative_hits.warn_once_per_file \
      --json "${1}"
}

# Redirect
test -n "${NO_REDIRECT:-}" \
  || {
       exec 5>&1 > coverage-out.txt 2>&1
  }

# Run 1 - Debug and self-log
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample || true
build/bin/sirtests || true
remove_sample || true
run_gcovr run-1.json
remove_coverage

# Run 2 - No plugins
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests || true
remove_sample || true
run_gcovr run-2.json
remove_coverage

# Run 3 - Perf test
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests --perf || true
remove_sample || true
run_gcovr run-3.json
remove_coverage

# Run 4 - Test help
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests --help || true
remove_sample || true
run_gcovr run-4.json
remove_coverage

# Run 5 - List tests
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests --list || true
remove_sample || true
run_gcovr run-5.json
remove_coverage

# Run 6 - Version check
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests --version || true
remove_sample || true
run_gcovr run-6.json
remove_coverage

# Run 7 - Invalid arguments to sirtest
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests --INVALID_ARGUMENTS || true
remove_sample || true
run_gcovr run-7.json
remove_coverage

# Run 8 - Bad `--only` without arguments to sirtest
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1 SIR_NO_PLUGINS=1
build/bin/sirexample || true
build/bin/sirtests --only || true
remove_sample || true
run_gcovr run-8.json
remove_coverage

# Run 9 - No debug and no self-log
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?}
build/bin/sirexample || true
build/bin/sirtests || true
remove_sample || true
run_gcovr run-9.json
remove_coverage

# Run 10 - Just self-log
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_SELFLOG=1
build/bin/sirexample || true
build/bin/sirtests || true
remove_sample || true
run_gcovr run-10.json
remove_coverage

# Run 11 - Interactive
printf '%s\n' '#!/usr/bin/env expect' > r.sh
# shellcheck disable=SC2129
printf '%s\n' 'spawn ./build/bin/sirtests --wait' >> r.sh
printf '%s\n' 'set timeout 999'                   >> r.sh
printf '%s\n' 'expect "press any key to exit..."' >> r.sh
printf '%s\n' 'send -- "\r"'                      >> r.sh
printf '%s\n' 'expect eof'                        >> r.sh
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_SELFLOG=1
build/bin/sirexample || true
chmod a+x r.sh
./r.sh || true
remove_sample || true
rm -f r.sh || true
run_gcovr run-11.json
remove_coverage

# Run 12 - Deny root
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample || true
fakeroot build/bin/sirtests || true
remove_sample || true
run_gcovr run-12.json
remove_coverage

# Run 13 - Fail some tests (no logs directory)
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample || true
rm -rf logs || true
build/bin/sirtests || true
remove_sample || true
run_gcovr run-13.json
remove_coverage

# Run 14 - Fail more.
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample || true
build/bin/sirtests --only 0000 || true
remove_sample || true
run_gcovr run-14.json
remove_coverage

# Run 15 - Leave logs
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample || true
build/bin/sirtests --leave-logs || true
remove_sample || true
run_gcovr run-15.json
remove_coverage

# Run 16 - Break readlink function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "long readlink() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-16.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 17 - Break clock_gettime function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int clock_gettime() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-17.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 18 - Break fstat and lstat functions
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int fstat() { return -1; }" > bad.c
printf '%s\n' "int lstat() { return -1; }" >> bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-18.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 19 - Break fputs function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_NO_SYSTEM_LOGGERS=1 SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int fputs() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-19.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 20 - Break fseek function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int fseek() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-20.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 21 - Break fputc function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int fputc() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-21.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 22 - Break gethostname function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int gethostname() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-22.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 23 - Break stat functions
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int stat() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-23.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 24 - Break pthread_join function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int pthread_join() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-24.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 25 - Break pthread_create function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int pthread_create() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-25.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 26 - Break dlopen function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int dlopen() { return 0; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-26.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 27 - Break dlsym function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int dlsym() { return 0; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-27.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 28 - Break readdir function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int readdir() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-28.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 29 - Break opendir function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int opendir() { return 0; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-29.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 30 - Break calloc function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "int calloc() { return 0; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-30.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 31 - Break time function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "long time() { return -1; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-31.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 32 - Break strndup function
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
printf '%s\n' "char *strndup() { return 0; }" > bad.c
gcc -shared -fPIC bad.c -o bad.so || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirexample || true
env LD_PRELOAD="$(pwd)/bad.so" build/bin/sirtests || true
remove_sample || true
run_gcovr run-32.json
remove_coverage
rm -f bad.c > /dev/null 2>&1
rm -f bad.so > /dev/null 2>&1

# Run 33 - Do --only file-archive-large
${DO_MAKE:-make} -j ${JOBS:?} clean
${DO_MAKE:-make} -j ${JOBS:?} SIR_DEBUG=1 SIR_SELFLOG=1
build/bin/sirexample || true
build/bin/sirtests --only file-archive-large || true
remove_sample || true
run_gcovr run-33.json
remove_coverage

# Undo redirect
test -n "${NO_REDIRECT:-}" \
  || {
       exec 1>&5
  }

# Show results
ls -l ./run-*.json || true

# Process results
MERGE_MODE="merge-use-line-0"
gcovr \
  --add-tracefile "run-*.json" \
  --merge-mode-functions="${MERGE_MODE:?}" -u -s \
  --gcov-ignore-parse-errors=negative_hits.warn_once_per_file \
  --html-details coverage-out.html
gcovr \
  --add-tracefile "run-*.json" \
  --merge-mode-functions="${MERGE_MODE:?}" -u \
  --gcov-ignore-parse-errors=negative_hits.warn_once_per_file \
  --coveralls coveralls.json
gcovr \
  --add-tracefile "run-*.json" \
  --merge-mode-functions="${MERGE_MODE:?}" -u -s \
  --gcov-ignore-parse-errors=negative_hits.warn_once_per_file

# Submit results
test -n "${NO_COVERALLS:-}" || \
    coveralls coveralls.json -r "${COVERALLS_REPO_TOKEN:?}"

# Cleanup
test -n "${NO_CLEANUP:-}" \
  || {
    cleanup_files || true
    rm -f ./run*.json > /dev/null 2>&1 || true
  }
