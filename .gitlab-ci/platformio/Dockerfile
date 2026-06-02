################################################################################
#
# Version: 2.2.6
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
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

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 10000

WORKDIR /

ARG URL

RUN set -x && \
      cd && \
      wget -O /tmp/get-platformio.py \
        https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py && \
      (python3 /tmp/get-platformio.py || true) && \
      export PATH="$HOME/.platformio/penv/bin:$PATH" && \
      pio --version && rm -f /tmp/get-platformio.py && \
      printf '%s\n' \
        "[env]" \
        "platform = espressif32" \
        "framework = arduino" \
        "[env:um_pros3]" \
        "board = um_pros3" > "$HOME/platformio.ini" && \
      pio pkg install -d . && \
      rm -f "$HOME/platformio.ini"

RUN printf '\n%s\n' "BUILD SUCCESS"
