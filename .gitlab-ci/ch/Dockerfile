# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL1="..." to override default local URL
# Use --build-arg URL2="..." to override default local URL

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL1
ARG URL2

RUN dnf --refresh -y upgrade && \
    dnf install -y ncurses-compat-libs libxcrypt-compat xdelta moreutils && \
    cd /tmp && \
    curl -fsSL ${URL1:-http://192.168.1.187:8088/~jhj/cache/chprofessional-8.5.linux64-4.18.0.intel.tar.gz} | \
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
    curl -fsSL ${URL2:-http://192.168.1.187:8088/~jhj/cache/ch-apply.tar.gz} | \
      gzip -dc | \
      tar xf - && \
    ./apply.sh && \
    rm -f *.vcdiff && \
    ( rm -f /opt/ch/license/* 2> /dev/null || true ) && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1 && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN cp -f /usr/local/ch/config/.chrc /home/user/.chrc && \
    cd && \
    ch -v

RUN printf '\n%s\n' "BUILD SUCCESS"
