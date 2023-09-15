# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN printf '%s\n' \
         "[viva64]" \
         "name=Viva64 - PVS-Studio - Stable" \
         "baseurl=https://cdn.pvs-studio.com/rpm" \
         "gpgcheck=0" \
         "enabled=1" > /etc/yum.repos.d/viva64.repo && \
  (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          pvs-studio) || \
  (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          --nogpgcheck ${URL:-http://192.168.1.187:8088/~jhj/cache/pvs-studio.x86_64.rpm}) \
&& \
  printf '%s\n' \
         "[viva64-beta]" \
         "name=Viva64 - PVS-Studio - Beta" \
         "baseurl=http://cdn.pvs-studio.com/beta/rpm" \
         "gpgcheck=0" \
         "enabled=1" \
         "skip_if_unavailable=True" > /etc/yum.repos.d/viva64-beta.repo && \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True upgrade \
&& \
      dnf -y clean all && \
      rm -rf ~/.cache && \
      rm -rf /var/tmp/* && \
      rm -rf /var/log/dnf* && \
      ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000
