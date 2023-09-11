# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf -y install elinks && \
    ( wget https://downloads.dlang.org/releases/2.x/2.105.1/dmd-2.105.1-0.fedora.x86_64.rpm || true ) && \
    ( wget "$(elinks -dump https://dlang.org/download.html | \
           awk '/\/releases\/.*fedora.*x86_64.rpm$/ { print $2 }' | \
	   head -1)" ) || true && \
    dnf --nogpgcheck -y install "$(ls -1 ./*.rpm | sort -rV | head -1)" && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
