################################################################################
#
# Author:    Jeffrey H. Johnson <trnsz@pobox.com>
# Copyright: Copyright (c) 2018-2023
# Version:   2.2.4
# License:   The MIT License (MIT)
#
################################################################################
#
# SPDX-License-Identifier: MIT
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

ARG URL

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && cd /tmp && \
    git clone -v https://git.sr.ht/\~jprotopopov/kefir && \
    cd kefir && \
    gmake clean && \
    gmake all -j "$(nproc 2> /dev/null || echo 1)" \
      CC="musl-gcc -fPIC -fpie" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC -fpie" \
      DBG="" \
      USE_SHARED="no" \
      USE_STATIC="yes" && \
    env \
      LC_ALL="C.UTF-8" \
        make test all -j "$(nproc 2> /dev/null || echo 1)" \
          OPT="-O3 -fPIC -fpie" \
          USE_SHARED="no" \
          USE_STATIC="yes" && \
    gmake clean && \
    gmake all  -j "$(nproc 2> /dev/null || echo 1)" \
      CC="musl-gcc -fPIC -fpie" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC -fpie" \
      DBG="" \
      USE_SHARED="no" \
      USE_STATIC="yes" && \
    mkdir -p /opt/kefir && \
    gmake install \
      DESTDIR="/" && \
    mv /opt/kefir /opt/kefir-musl && \
    mkdir -p /opt/kefir && \
    gmake clean && \
    gmake all -j "$(nproc 2> /dev/null || echo 1)" \
      CC="gcc -fPIC" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC" \
      DBG="" && \
    env \
      LC_ALL="C.UTF-8" \
        make test all -j "$(nproc 2> /dev/null || echo 1)" \
          OPT="-O3 -fPIC" && \
    gmake clean && \
    gmake all  -j "$(nproc 2> /dev/null || echo 1)" \
      CC="gcc -fPIC" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC" \
      DBG="" && \
    gmake install \
      DESTDIR="/" && \
    cd && \
    rm -rf /tmp/kefir && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true ) && \
    touch /OK

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
