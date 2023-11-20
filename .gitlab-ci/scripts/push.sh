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
printf 'Pushing %s image ...\n' "$(basename "$(pwd -P)")"

# shellcheck disable=SC2312
docker push registry.gitlab.com/libsir/libsir/"$(basename "$(pwd -P)")":latest

printf '%s\n' "Complete."
