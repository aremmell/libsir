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

RUN set -x && \
    cd /tmp && \
    git clone https://github.com/johnsonjh/pcc-revived.git \
      -b johnsonjh/rawhide && \
    cd pcc-revived/pcc-libs && \
    mkdir -p /opt/pcc && \
    ./configure --prefix=/opt/pcc && \
    gmake -j "$(nproc 2> /dev/null || echo 1)" && \
    gmake install && \
    cd ../pcc && \
    env CFLAGS="-Wno-int-conversion" \
      ./configure --prefix=/opt/pcc \
        --enable-native --enable-tls --disable-Werror && \
    gmake -j "$(nproc 2> /dev/null || echo 1)" && \
    gmake install && \
    mkdir -p /opt/pcc/include && \
    ln -fs /opt/pcc/lib/pcc/x86_64-pc-linux-gnu/1.2.0.DEVEL/include/ \
      /opt/pcc/include/pcc && \
    cd && \
    rm -rf /tmp/pcc && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    touch /OK

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
