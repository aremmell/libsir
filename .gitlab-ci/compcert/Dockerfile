# SPDX-License-Identifier: MIT
# Copyright (c) 2018-2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>

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
    mkdir -p /opt/compcert && \
    chown -R 10000:10000 /opt/src && \
    chown -R 10000:10000 /opt/compcert*

USER 10000

RUN set -x && \
    export OPAMYES="true" && \
    opam init --disable-sandboxing --bare -ni && \
    opam switch create compcert 5.4.1 && \
    eval $(opam env --switch=compcert) && \
    opam repo add rocq-released https://rocq-prover.org/opam/released && \
    opam pin add -n coq 9.1.1 && \
    opam install menhir coq coq-flocq && \
    coqc --version && \
    cd /opt/src && \
    git clone https://github.com/absint/compcert && \
    cd compcert && \
    ./configure \
      -ignore-coq-version \
      -ignore-ocaml-version \
      -use-external-Flocq \
      -prefix /opt/compcert x86_64-linux && \
    make -j "$(nproc 2> /dev/null || echo 1)" && \
    make install && \
    make clean && \
    opam switch remove compcert --yes && \
    rm -rf /opt/src/* && \
    rm -rf "${HOME:-}"/.opam

RUN printf '\n%s\n' "BUILD SUCCESS"
