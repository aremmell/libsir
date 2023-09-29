# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL="..." to override default
# android-ndk.tar.gz is a repacked archive of the Android NDK (as android-ndk/*)

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /opt

ARG URL

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && \
    curl -fsSL ${URL:-http://192.168.1.187:8088/~jhj/cache/android-ndk.tar.gz} | pigz -dc | tar xvf - && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
