################################################################################
#
# Version: 2.2.5
#
################################################################################
#
# SPDX-License-Identifier: ISC
#
# Copyright (c) 2018-2019 Martin Storsjo
# Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
################################################################################

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

RUN set -x && \
    export WINEDEBUG=-all && \
    dnf --refresh -y install --setopt=install_weak_deps=True \
        cabextract \
        libwbclient.i686 \
        libwbclient.x86_64 \
        msitools \
        samba-winbind \
        unzip \
        zip && \
    dnf -y clean all && \
    wine64 wineboot --init && \
    while pgrep wineserver > /dev/null; do sleep 10; done

WORKDIR /opt/msvc

COPY msvc-wine/lowercase msvc-wine/fixinclude msvc-wine/install.sh msvc-wine/vsdownload.py msvc-wine/msvctricks.cpp ./
COPY msvc-wine/wrappers/* ./wrappers/

RUN set -x && \
    export WINEDEBUG=-all && \
    stdbuf -o L \
        env PYTHONUNBUFFERED=1 ./vsdownload.py \
        --accept-license \
        --dest /opt/msvc && \
    sh -x ./install.sh /opt/msvc && \
    rm -f lowercase fixinclude install.sh vsdownload.py && \
    rm -rf wrappers

COPY msvc-wine/msvcenv-native.sh /opt/msvc

RUN set -x && \
    curl \
      -fsSL https://raw.githubusercontent.com/Winetricks/winetricks/master/src/winetricks \
        > /usr/bin/winetricks && chmod a+x /usr/bin/winetricks

RUN set -x && \
    ln -s /opt/msvc /opt/MSVC

RUN set -x && \
    rm -f /opt/msvc/msvctricks.cpp && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf*

WORKDIR /

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
