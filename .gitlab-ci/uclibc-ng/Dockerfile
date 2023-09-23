# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL="..." to override default

# x86_64-pc-linux-uclibc.tar.bz2 is a toolchain built
# using crosstool-ng with the included .config file.

FROM registry.gitlab.com/libsir/libsir/base:latest

WORKDIR /

ARG URL

USER 0

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true )

USER 10000

RUN cd ~user && \
    mkdir -p x-tools && \
    cd x-tools && \
    curl "${URL:-http://192.168.1.187:8088/~jhj/cache/x86_64-pc-linux-uclibc.tar.bz2}" | \
      bzip2 -dc | \
      tar xvf - && \
    rm -f x86_64-pc-linux-uclibc/build.log.lz && cd

USER 10000
