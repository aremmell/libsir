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

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN set -x && \
    cd /tmp && \
    git clone -v https://git.sr.ht/\~jprotopopov/kefir && \
    cd kefir && \
    mkdir -p /opt/kefir && \
    gmake clean && \
    env LC_ALL="C.UTF-8" gmake all -j "$(nproc 2> /dev/null || echo 1)" \
      CC="musl-gcc" \
      EXTRA_CFLAGS="-static -march=x86-64 -DNDEBUG -fPIC -fpie" \
      PROFILE="release" \
      LDFLAGS="-static" \
      USE_SHARED="no" \
      USE_STATIC="yes" && \
    gmake install \
      DESTDIR="/" && \
    mv /opt/kefir /opt/kefir-musl && \
    mkdir -p /opt/kefir && \
    gmake clean && \
    env LC_ALL="C.UTF-8" gmake all test -j "$(nproc 2> /dev/null || echo 1)" \
      CC="gcc" \
      EXTRA_CFLAGS="-march=x86-64 -DNDEBUG -fPIC" \
      PROFILE="release" && \
    gmake install \
      DESTDIR="/" && \
    cd && \
    rm -rf /tmp/kefir && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    touch /OK

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
