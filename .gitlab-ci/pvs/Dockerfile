################################################################################
#
# Version: 2.2.6
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

ARG URL

RUN set -x && \
    printf '%s\n' \
         "[viva64]" \
         "name=Viva64 - PVS-Studio - Stable" \
         "baseurl=https://cdn.pvs-studio.com/rpm" \
         "gpgcheck=0" \
         "skip_if_unavailable=True" \
         "enabled=1" > /etc/yum.repos.d/viva64.repo && \
    (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          pvs-studio) || \
    (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          "${URL:-http://192.168.1.187:8088/~jhj/cache/pvs-studio.x86_64.rpm}") && \
    printf '%s\n' \
         "[viva64-beta]" \
         "name=Viva64 - PVS-Studio - Beta" \
         "baseurl=http://cdn.pvs-studio.com/beta/rpm" \
         "gpgcheck=0" \
         "enabled=1" \
         "skip_if_unavailable=True" > /etc/yum.repos.d/viva64-beta.repo && \
    (dnf -y --refresh --setopt=install_weak_deps=True \
       --setopt=keepcache=True upgrade pvs-studio || true) && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf*

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
