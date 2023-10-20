# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Jeffrey H. Johnson <trnsz@pobox.com>

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN dnf --refresh -y upgrade && \
    sudo -i -u user sh -xc '( \
      mkdir -p "${HOME:-}/x-tools" && \
      cd "${HOME:-}/x-tools" && \
      curl -fsSL ${URL:-http://192.168.1.187:8088/~jhj/cache/wrsdk-vxworks7-qemu-1.12.1.tar.bz2} | \
        bzip2 -dc | \
          tar axvf - )' && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1 && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
