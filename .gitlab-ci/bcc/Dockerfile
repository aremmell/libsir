# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN cd /opt && \
    wget http://192.168.1.187:8088/~jhj/cache/BCC102.zip && \
    atool -x BCC102.zip && \
    env rm -f BCC102.zip && \
    ln -s BCC102 bcc

USER 10000
