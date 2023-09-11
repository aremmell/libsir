# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN cd /tmp && \
    git clone -v https://git.sr.ht/\~jprotopopov/kefir && \
    cd kefir && \
    gmake clean && \
    gmake all -j "$(nproc 2> /dev/null || echo 1)" \
      CC="musl-gcc -fPIC -fpie" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC -fpie" \
      DBG="" \
      USE_SHARED="no" \
      USE_STATIC="yes" && \
    env \
      LC_ALL="C.UTF-8" \
        make test all -j "$(nproc 2> /dev/null || echo 1)" \
          OPT="-O3 -fPIC -fpie" \
          USE_SHARED="no" \
          USE_STATIC="yes" && \
    gmake clean && \
    gmake all  -j "$(nproc 2> /dev/null || echo 1)" \
      CC="musl-gcc -fPIC -fpie" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC -fpie" \
      DBG="" \
      USE_SHARED="no" \
      USE_STATIC="yes" && \
    mkdir -p /opt/kefir && \
    gmake install \
      DESTDIR="/" && \
    mv /opt/kefir /opt/kefir-musl && \
    mkdir -p /opt/kefir && \
    gmake clean && \
    gmake all -j "$(nproc 2> /dev/null || echo 1)" \
      CC="gcc -fPIC" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC" \
      DBG="" && \
    env \
      LC_ALL="C.UTF-8" \
        make test all -j "$(nproc 2> /dev/null || echo 1)" \
          OPT="-O3 -fPIC" && \
    gmake clean && \
    gmake all  -j "$(nproc 2> /dev/null || echo 1)" \
      CC="gcc -fPIC" \
      OPT="-O3 -march=x86-64 -DNDEBUG -fPIC" \
      DBG="" && \
    gmake install \
      DESTDIR="/" && \
    cd && \
    rm -rf /tmp/kefir && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true) && \
    touch /OK

USER 10000
