# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL="..." to override default
# android-ndk.tar.gz is a repacked archive of the Android NDK (as android-ndk/*)

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /opt

ARG URL

RUN curl -fsSL ${URL:-http://192.168.1.187:8088/~jhj/cache/android-ndk.tar.gz} | pigz -dc | tar xvf - && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
