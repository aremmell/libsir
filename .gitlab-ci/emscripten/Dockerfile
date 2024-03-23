################################################################################
#
# Version: 2.2.5
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
################################################################################

################################################################################
# NOTE: Recommend adding 'LimitNOFILE=infinity' to docker.service and
#       systemctl daemon-reload and then restarting docker.service.
################################################################################

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN set -x && \
    mkdir -p /home/linuxbrew && \
    chown user:user -R /home/linuxbrew && \
    export UNATTENDED=1 && \
    export NONINTERACTIVE=1 && \
    cd /tmp && \
    dnf -y --best --allowerasing --skip-broken upgrade nodejs || true && \
    wget https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh && \
    sudo -s -u user bash ./install.sh && \
    rm -f ./install.sh && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; env HOMEBREW_NO_ENV_HINTS=1 HOMEBREW_NO_INSTALL_CLEANUP=1 brew developer on )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; env HOMEBREW_NO_ENV_HINTS=1 HOMEBREW_NO_INSTALL_CLEANUP=1 brew update -v )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; env HOMEBREW_NO_ENV_HINTS=1 HOMEBREW_NO_INSTALL_CLEANUP=1 brew upgrade -v )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; env HOMEBREW_NO_ENV_HINTS=1 HOMEBREW_NO_INSTALL_CLEANUP=1 brew install -v emscripten )" && \
    sudo -s -u user bash -c "( set -e; eval \"$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)\"; env HOMEBREW_NO_ENV_HINTS=1 HOMEBREW_NO_INSTALL_CLEANUP=1 brew cleanup -s --prune=all || true)" && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    cd

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
