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

FROM ubuntu:20.04 AS builder

USER 0

WORKDIR /

RUN set -x && \
    export DEBIAN_FRONTEND=noninteractive && \
    export CLANG_HOME=/usr/lib/llvm-11 && \
    apt update -y && \
    apt upgrade -y && \
    apt full-upgrade -y && \
    apt install -y \
        bash \
        binutils \
        bison \
        build-essential \
        clang-11 \
        cmake \
        file \
        flex \
        g++ \
        gawk \
        gcc \
        git \
        libc6-dev \
        libclang-11-dev \
        libncurses-dev \
        libtinfo-dev \
        llvm-11-dev \
        make \
        perl \
        zlib1g-dev && \
    git clone https://github.com/open64-compiler/open64 && \
    mkdir -p /build && \
    cd /build && \
    /open64/configure --prefix=/opt/open64 --disable-jfe --disable-multilib --with-build-optimize=DEBUG --disable-fortran && \
    make build -j $(nproc) && \
    make lib -j 1 && \
    make install

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

COPY --from=builder /opt/open64 /opt/open64

RUN set -x && \
    cd /opt/open64/lib/gcc-lib/x86_64-open64-linux/5.0 && \
    for i in "$(dirname "$(gcc -print-file-name=crti.o)")"/*.o; do ln -s $i . || :; done && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
