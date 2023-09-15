# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL="..." to override default OrangeC ZIP (6.0.71.10 or later)

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN cd /opt && \
    wget ${URL:-http://192.168.1.187:8088/~jhj/cache/OrangeC.zip} && \
    atool -x OrangeC.zip && \
    env rm OrangeC.zip && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
