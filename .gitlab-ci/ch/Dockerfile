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

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL1
ARG URL2

RUN set -x && \
    dnf install -y ncurses-compat-libs libxcrypt-compat xdelta moreutils && \
    cd /tmp && \
    curl -fsSL "${URL1:-http://192.168.1.187:8088/~jhj/cache/chprofessional-8.5.linux64-4.18.0.intel.tar.gz}" | \
        gzip -dc | \
        tar xf - && \
    cd chprofessional-8.5.linux64-4.18.0.intel && \
    sed -i 's/^more .*license.txt$//' install.sh && \
    sed -i 's/read TARGET$//' install.sh && \
    sed -i 's/read check$//' install.sh && \
    echo 'true' > license.gen && \
    chmod a+x license.gen && \
    env check="y" TARGET="/usr/local/ch8.5" ./install.sh && \
    cp -f /usr/local/ch/config/.chrc /root/.chrc && \
    cd && \
    rm -rf /tmp/chprofessional-8.5.linux64-4.18.0.intel && \
    ln -fs /usr/local/ch /opt/ch && \
    curl -fsSL "${URL2:-http://192.168.1.187:8088/~jhj/cache/ch-apply.tar.gz}" | \
      gzip -dc | \
      tar xf - && \
    ./apply.sh && \
    rm -f ./*.vcdiff && \
    ( rm -f /opt/ch/license/* 2> /dev/null || true ) && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1

USER 10000

RUN set -x && cp -f /usr/local/ch/config/.chrc /home/user/.chrc && \
    cd && \
    ch -v

RUN printf '\n%s\n' "BUILD SUCCESS"
