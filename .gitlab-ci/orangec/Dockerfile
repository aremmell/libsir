# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf -q -y install tmux screen zip unzip atool && dnf -q clean all && cd /opt && wget http://192.168.1.187:8088/~jhj/cache/OrangeC.zip && atool -x OrangeC.zip && env rm OrangeC.zip

USER 10000
