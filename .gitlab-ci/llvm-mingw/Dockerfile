# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM mstorsjo/llvm-mingw:latest

USER 0

RUN useradd -m -U -G 0 --uid 10000 -c "CI/CD User" user

WORKDIR /

USER 10000
