# SPDX-License-Identifier: MIT
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN set -x && \
    dnf -y install opam --setopt=install_weak_deps=True && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    mkdir -p /opt/src && \
    mkdir -p /opt/chamois-compcert && \
    mkdir -p /opt/chamois-compcert-i386 && \
    chown -R 10000:10000 /opt/src && \
    chown -R 10000:10000 /opt/chamois-compcert*

USER 10000

RUN set -x && \
    export OPAMYES="true" && \
    opam init --reinit -ni && \
    eval $(opam env --switch=default) && \
    opam update && \
    opam upgrade && \
    eval $(opam env) && \
    opam switch create chamois-compcert 4.14.2 && \
    eval $(opam env --switch=chamois-compcert) && \
    opam pin add coq 8.16.1 && \
    eval $(opam env) && \
    opam install menhir coq && \
    eval $(opam env) && \
    coqc --version && \
    cd /opt/src && \
    git clone https://gricad-gitlab.univ-grenoble-alpes.fr/certicompil/Chamois-CompCert.git && \
    cd Chamois-CompCert && \
    opam switch chamois-compcert && \
    eval $(opam env) && \
    ./configure -prefix /opt/chamois-compcert x86_64-linux && \
    make -j "$(nproc 2> /dev/null || echo 1)" && \
    make install && \
    make clean && \
    ./configure -prefix /opt/chamois-compcert-i386 i386-linux && \
    make -j "$(nproc 2> /dev/null || echo 1)" && \
    make install && \
    make clean && \
    rm -rf /opt/src/*

RUN printf '\n%s\n' "BUILD SUCCESS"
