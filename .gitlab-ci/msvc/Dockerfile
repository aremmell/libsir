# SPDX-License-Identifier: ISC and MIT
# Copyright (c) 2018-2019 Martin Storsjo
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && \
    dnf --refresh -y install --setopt=install_weak_deps=True \
        msitools \
        cabextract \
        zip \
        unzip \
        libwbclient.i686 \
        libwbclient.x86_64 && \
    wine64 wineboot --init && \
    while pgrep wineserver > /dev/null; do sleep 10; done

WORKDIR /opt/msvc

COPY lowercase fixinclude install.sh vsdownload.py msvctricks.cpp ./
COPY wrappers/* ./wrappers/

RUN stdbuf -o L \
        env PYTHONUNBUFFERED=1 ./vsdownload.py \
        --accept-license \
        --dest /opt/msvc && \
    sh -x ./install.sh /opt/msvc && \
    rm -f lowercase fixinclude install.sh vsdownload.py && \
    rm -rf wrappers

COPY msvcenv-native.sh /opt/msvc

RUN curl \
      -fsSL https://raw.githubusercontent.com/Winetricks/winetricks/master/src/winetricks \
        > /usr/bin/winetricks && chmod a+x /usr/bin/winetricks

RUN ln -s /opt/msvc /opt/MSVC

RUN dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true )

WORKDIR /

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
