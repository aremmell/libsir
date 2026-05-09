################################################################################
#
# Version: 2.2.6
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
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

RUN set -x && \
    cd /tmp && \
    wget -q https://downloads.dlang.org/releases/2.x/2.112.0/dmd.2.112.0.linux.tar.xz && \
    cd /opt && \
    tar axvf /tmp/dmd.2.112.0.linux.tar.xz && \
    rm -f dmd.2.112.0.linux.tar.xz && \
    ln -s /opt/dmd2/linux/bin64/ddemangle /usr/bin/ddemangle && \
    ln -s /opt/dmd2/linux/bin64/dmd /usr/bin/dmd && \
    ln -s /opt/dmd2/linux/bin64/dub /usr/bin/dub && \
    ln -s /opt/dmd2/linux/bin64/dustmite /usr/bin/dustmite && \
    ln -s /opt/dmd2/linux/bin64/rdmd /usr/bin/rdmd && \
    rm -f ./*.rpm* && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf*

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
