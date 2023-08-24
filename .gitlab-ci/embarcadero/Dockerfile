# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL="..." to override default

FROM registry.gitlab.com/libsir/libsir/base:latest

WORKDIR /

ARG URL

RUN cd ~user && \
    export WINEDEBUG=-all && \
    ( ( yes | wine64 -h ) || true ) && \
    cd .wine/drive_c/Program\ Files\ \(x86\)/ && \
    mkdir -p Embarcadero/Studio && \
    cd Embarcadero/Studio && \
    curl -fsSL ${URL:-http://192.168.1.187:8088/~jhj/cache/EMB22.tar} | tar xvf -

USER 10000
