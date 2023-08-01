# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN rpm --import https://developer.download.nvidia.com/hpc-sdk/rhel/RPM-GPG-KEY-NVIDIA-HPC-SDK && \
    curl -fsSL https://developer.download.nvidia.com/hpc-sdk/rhel/nvhpc.repo > /etc/yum.repos.d/nvhpc.repo && \
    dnf --refresh -q -y install nvhpc && \
    dnf -q -y clean all

USER 10000
