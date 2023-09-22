# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN dnf --nogpgcheck --refresh -y upgrade --setopt=install_weak_deps=True && \
    ( rpm --import https://yum.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB ) && \
    printf '%s\n' \
      "[oneAPI]" \
      "name=Intel® oneAPI repository" \
      "baseurl=https://yum.repos.intel.com/oneapi" \
      "enabled=1" \
      "gpgcheck=1" \
      "repo_gpgcheck=1" \
      "gpgkey=https://yum.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB" \
        > /etc/yum.repos.d/oneAPI.repo && \
    dnf -y --nogpgcheck --refresh --setopt=install_weak_deps=True upgrade && \
    dnf -y --nogpgcheck --refresh --setopt=install_weak_deps=True install \
      intel-basekit \
      intel-hpckit \
      intel-oneapi-python && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1 && \
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true ) && cd

USER 10000
