# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf -q -y install gcc-gdc ldc elinks && \
    dnf -q -y --nogpgcheck install \
        "$(elinks -dump https://dlang.org/download.html | \
           awk '/\/releases\/.*fedora.*x86_64.rpm$/ { print $2 }' | \
	   head -1)" && \
    dnf -q -y clean all

USER 10000
