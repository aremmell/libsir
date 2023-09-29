# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && mkdir -p /home/linuxbrew && \
    chown user:user -R /home/linuxbrew && \
    export UNATTENDED=1 && \
    export NONINTERACTIVE=1 && \
    cd /tmp && \
    wget https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh && \
    sudo -s -u user bash ./install.sh && \
    rm -f ./install.sh && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; brew developer on )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; brew update -v )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; brew upgrade -v )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; brew install --display-times -v emscripten )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; brew cleanup -s --prune=all )" && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true) && \
    cd

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
