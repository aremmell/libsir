# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf --refresh -y upgrade --setopt=install_weak_deps=True && \
    rpm --import https://developer.download.nvidia.com/hpc-sdk/rhel/RPM-GPG-KEY-NVIDIA-HPC-SDK && \
    curl -fsSL https://developer.download.nvidia.com/hpc-sdk/rhel/nvhpc.repo > /etc/yum.repos.d/nvhpc.repo && \
    dnf --refresh -y install nvhpc --setopt=install_weak_deps=True && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
