# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Jeffrey H. Johnson <trnsz@pobox.com>

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && \
    cd /tmp && \
    wget https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh && \
    chmod a+x Miniconda3-latest-Linux-x86_64.sh && \
    export PATH="/opt/anaconda/bin:/opt/anaconda/sbin:${PATH:-}" && \
    ./Miniconda3-latest-Linux-x86_64.sh -b -t -p /opt/anaconda -t && \
    rm -f ./Miniconda3-latest-Linux-x86_64.sh && \
    conda build purge && \
    conda update --update-all --verbose --yes && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true) && \
    cd

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
