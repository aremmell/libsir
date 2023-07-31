# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# android-ndk.tar.gz is a repacked archive of the Android NDK (as android-ndk/*)

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /opt

RUN curl -fsSL http://192.168.1.187:8088/~jhj/cache/android-ndk.tar.gz | pigz -dc | tar xvf -

USER 10000
