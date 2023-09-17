# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM mstorsjo/llvm-mingw:latest

USER 0

RUN apt -y update && \
    apt -y upgrade && \
    apt -y full-upgrade && \
    apt -y install \
	dos2unix \
	screen \
        tmux \
	git \
	libfmt-dev \
	libboost-all-dev \
        wine && \
    yes | dpkg --add-architecture i386 && \
    apt -y update && \
    apt -y upgrade && \
    apt -y full-upgrade && \
    apt -y install wine32 && \
    git clone https://gitlab.com/saalen/ansifilter.git && \
      cd ansifilter && \
      make && \
      make install && \
      cd .. && \
      rm -rf ansifilter && \
    ansifilter --version && \
    apt -y clean all && \
    useradd -m -U -G 0 --uid 10000 -c "CI/CD User" user && cd

WORKDIR /

USER 10000
