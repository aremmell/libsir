# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL1="..." to override default libnsl link
# Use --build-arg URL2="..." to override default Oracle link
# oracle126.tar.gz is an archive of Oracle Studio 12.6

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL1
ARG URL2

RUN ( dnf --refresh -y install libnsl2.x86_64 libnsl2.i686 libnsl.i686 libnsl.x86_64 ) && \
    ( ( (rpm -qa | grep 'libnsl-.*x86_64') || \
         dnf -y install ${URL1:-http://192.168.1.187;8088/~jhj/cache/libnsl-2.38-1.fc39.x86_64.rpm}) || \
    true ) && \
    curl -fsSL ${URL2:-http://192.168.1.187:8088/~jhj/cache/oracle126.tar.gz} | \
        pigz -dc | \
	tar xvf - && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
