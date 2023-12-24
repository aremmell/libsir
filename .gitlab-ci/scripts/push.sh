#!/usr/bin/env sh

# Docker push script
################################################################################
#
# Version: 2.2.5
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
