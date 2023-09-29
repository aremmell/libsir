# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && \
    dnf -y install elinks --setopt=install_weak_deps=True && \
    (wget https://downloads.dlang.org/releases/2.x/2.105.2/dmd-2.105.2-0.fedora.x86_64.rpm || true) && \
    (wget "$(elinks -dump https://dlang.org/download.html | \
           awk '/\/releases\/.*fedora.*x86_64.rpm$/ { print $2 }' | \
	   head -1)" || true) && \
    dnf --nogpgcheck -y install "$(ls -1 ./*.rpm | sort -rV | head -1)" --setopt=install_weak_deps=True && \
    rm -f ./*.rpm* && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
