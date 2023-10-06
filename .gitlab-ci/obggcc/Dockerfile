# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf --refresh -y upgrade && \
    cd /tmp && \
    wget -vS \
      https://github.com/AmanoTeam/obggcc/releases/download/0.5/x86_64-unknown-linux-gnu.tar.xz && \
    cd && sudo -s -u user sh -xc \
      'cd ~ && mkdir x-tools && cd x-tools && tar axvf /tmp/x86_64-unknown-linux-gnu.tar.xz' && \
    env rm -f /tmp/x86_64-unknown-linux-gnu.tar.xz && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1 && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
