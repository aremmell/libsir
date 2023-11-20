# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Jeffrey H. Johnson <trnsz@pobox.com>

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && cd /opt && \
    wget ${URL:-http://192.168.1.187:8088/~jhj/cache/OrangeC.zip} && \
    atool -x OrangeC.zip && \
    env rm OrangeC.zip && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
