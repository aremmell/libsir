#!/usr/bin/env sh

# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

printf '%s\n' "Sanity check ..."
test -f Dockerfile ||
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

printf '%s\n' "Building base image ..."
docker build --no-cache --squash -t registry.gitlab.com/libsir/libsir/base:latest .

printf '%s\n' "Complete."
