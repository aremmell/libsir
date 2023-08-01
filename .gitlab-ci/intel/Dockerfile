# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# BaseKit_offline.sh and HPCKit_offline.sh are offline installers from Intel.

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN cd /tmp && wget -q http://192.168.1.187:8088/~jhj/cache/BaseKit_offline.sh && chmod a+x ./BaseKit_offline.sh && ./BaseKit_offline.sh -a -s --eula accept && rm -f BaseKit_offline.sh
RUN cd /tmp && wget -q http://192.168.1.187:8088/~jhj/cache/HPCKit_offline.sh && chmod a+x ./HPCKit_offline.sh && ./HPCKit_offline.sh -a -s --eula accept && rm -f HPCKit_offline.sh

USER 10000
