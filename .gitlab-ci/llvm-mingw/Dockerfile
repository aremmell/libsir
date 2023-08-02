# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM mstorsjo/llvm-mingw:latest

USER 0

RUN apt -q -y update && \
    apt -q -y upgrade && \
    apt -q -y full-upgrade && \
    apt -q -y install \
	dos2unix \
	screen \
        tmux \
	git \
        wine && \
    yes | dpkg --add-architecture i386 && \
    apt -q -y update && \
    apt -q -y upgrade && \
    apt -q -y full-upgrade && \
    apt -q -y install wine32 && \
    git clone https://gitlab.com/saalen/ansifilter.git && \
      cd ansifilter && \
      make && \
      make install && \
      cd .. && \
      rm -rf ansifilter && \
    ansifilter --version && \
    apt -q -y clean all && \
    useradd -m -U -G 0 --uid 10000 -c "CI/CD User" user

WORKDIR /

USER 10000
