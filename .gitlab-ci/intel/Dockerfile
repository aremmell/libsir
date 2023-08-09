# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# BaseKit_offline.sh and HPCKit_offline.sh are offline installers from Intel.

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

RUN ( rpm --import https://yum.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB || true ) && \
    printf '%s\n' \
      "[oneAPI]" \
      "name=Intel® oneAPI repository" \
      "baseurl=https://yum.repos.intel.com/oneapi" \
      "enabled=1" \
      "gpgcheck=1" \
      "repo_gpgcheck=1" \
      "gpgkey=https://yum.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS.PUB" \
        > /etc/yum.repos.d/oneAPI.repo && \
    dnf -y --refresh --setopt=install_weak_deps=True install \
      intel-basekit \
      intel-hpckit && \
  dnf -y clean all && \
  rm -rf ~/.cache && \
  rm -rf /var/tmp/* && \
  rm -rf /var/log/dnf* && \
  ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
