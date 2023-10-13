#!/usr/bin/env sh

# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Jeffrey H. Johnson <trnsz@pobox.com>

printf '%s\n' "Sanity check ..."
test -f Dockerfile 2> /dev/null ||
  {
    printf '%s\n' "ERROR: No Dockerfile in current working directory."
    exit 1
  }

printf '%s\n' "Finding docker ..."
command -v docker > /dev/null 2>&1 ||
  {
    printf '%s\n' "ERROR: No docker command found in PATH."
    exit 1
  }

printf '%s\n' "Testing docker ..."
docker run --rm -it "hello-world:latest" > /dev/null ||
  {
    printf '%s\n' "ERROR: Docker cannot run containers."
    exit 1
  }

# shellcheck disable=SC2312
printf 'Building %s image ...\n' "$(basename "$(pwd -P)")"

TMPLOG="$(mktemp 2> /dev/null)" ||
  {
    printf '%s\n' "ERROR: Unable to create temporary file with mktemp."
    exit 1
  }

# shellcheck disable=SC2312
stdbuf -o L \
    docker build \
      --no-cache --squash -t \
        registry.gitlab.com/libsir/libsir/"$(basename "$(pwd -P)")":latest . "${@}" 2>&1 | \
    stdbuf -o L tee "${TMPLOG:?}"

RESULT="$(tail "${TMPLOG:?}" 2> /dev/null)"

echo "${RESULT:-}" | grep -iq "BUILD SUCCESS$" 2> /dev/null ||
  {
    printf '%s\n' "ERROR: Unable to determine if docker build successfully completed."
    rm -f "${TMPLOG:-}" > /dev/null 2>&1 || true
    exit 1
  }

rm -f "${TMPLOG:-}" > /dev/null 2>&1 || true

printf '%s\n' "Complete."
