# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# x86_open64-4.5.2.1-1.x86_64.rpm is AMD Open64 4.5.2.1 RPM

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf --nogpgcheck -y install http://192.168.1.187:8088/~jhj/cache/x86_open64-4.5.2.1-1.x86_64.rpm

USER 10000
