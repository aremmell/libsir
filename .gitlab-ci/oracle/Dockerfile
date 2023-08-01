# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# oracle126.tar.gz is an archive of Oracle Studio 12.6

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN curl -fsSL http://192.168.1.187:8088/~jhj/cache/oracle126.tar.gz | pigz -dc | tar xvf -

USER 10000
