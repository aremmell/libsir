# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (c) 2018-current Ryan M. Lederman
##
#################
#  dnf -q -y --setopt=install_weak_deps=True --setopt=keepcache=True install --no-docs wine \
#&& \
#################
##
FROM quay.io/fedora/fedora:rawhide
WORKDIR /
RUN \
  (sed -i 's/tsflags=nodocs//' /etc/dnf/dnf.conf || true) \
&& \
  dnf -q -y --refresh --no-docs update \
&& \
  dnf -q -y --no-docs install dnf-plugins-core dnf-utils \
&& \
  dnf -q -y --refresh --no-docs update \
&& \
  dnf -q -y --no-docs install rpmconf \
&& \
  (rpmconf --all --unattended use_maintainer || true) \
&& \
  (rpmconf --all --unattended use_maintainer --clean || true) \
&& \
  dnf -q -y --refresh --no-docs update \
&& \
  dnf -q -y --setopt=install_weak_deps=True --setopt=keepcache=True --no-docs install \
      advancecomp \
      bear \
      ca-certificates \
      ccache \
      clang \
      clang-analyzer \
      clang-tools-extra \
      cmake \
      cppcheck \
      cppcheck-htmlreport \
      cppi \
      curl \
      doxygen \
      expect \
      fakeroot \
      flawfinder \
      g++ \
      gawk \
      gcc \
      musl-devel \
      musl-gcc \
      musl-libc \
      musl-libc-static \
      gcovr \
      git \
      glibc-static \
      lld \
      make \
      openssl \
      pigz \
      python3-setuptools \
      reuse \
      symlinks \
      time \
      valgrind \
      wget \
&& \
  (curl -fsSL https://coveralls.io/coveralls-linux.tar.gz | tar -zxv -C /usr/bin) \
&& \
  dnf -q -y --setopt=install_weak_deps=True --setopt=keepcache=True --no-docs install \
      mingw-binutils-generic \
      mingw32-binutils \
      mingw32-cpp \
      mingw32-crt \
      mingw32-dlfcn \
      mingw32-dlfcn-static \
      mingw32-gcc \
      mingw32-headers \
      mingw32-winpthreads \
      mingw32-winpthreads-static \
      mingw64-binutils \
      mingw64-cpp \
      mingw64-crt \
      mingw64-dlfcn \
      mingw64-dlfcn-static \
      mingw64-gcc \
      mingw64-headers \
      mingw64-winpthreads \
      mingw64-winpthreads-static \
      mingw-w64-tools \
      ucrt64-binutils \
      ucrt64-cpp \
      ucrt64-crt \
      ucrt64-gcc \
      ucrt64-headers \
      ucrt64-winpthreads \
      ucrt64-winpthreads-static \
&& \
  (yes | dnf -y copr enable yselkowitz/cygwin fedora-rawhide-x86_64) \
&& \
  dnf -q -y --setopt=install_weak_deps=True --setopt=keepcache=True --no-docs install \
      cygport \
      cygwin32 \
      cygwin32-binutils \
      cygwin32-gcc \
      cygwin64 \
      cygwin64-binutils \
      cygwin64-gcc \
      cygwin-w32api-headers \
&& \
  mkdir -p /opt && \
  cd /opt && \
  git clone -v --depth=1 https://github.com/rui314/chibicc && \
  cd chibicc && \
  (sed -i 's/^CFLAGS=/CFLAGS+=/' Makefile || true) && \
  env CFLAGS='-O2 -flto=auto' LDFLAGS='-flto=auto' make test && \
  make clean && \
  env CFLAGS='-O2 -flto=auto' LDFLAGS='-flto=auto' make && \
  (rm -rf ./test ./.git ./*.o ./*.c ./*.h .gitignore LICENSE Makefile README.md || true) && \
  strip chibicc && \
  cd / \
&& \
  printf '%s\n' \
         "[viva64]" \
	 "name=Viva64 - PVS-Studio - Stable" \
         "baseurl=https://cdn.pvs-studio.com/rpm" \
	 "gpgcheck=0" \
	 "enabled=1" > /etc/yum.repos.d/viva64.repo && \
  dnf -q -y --setopt=install_weak_deps=True --setopt=keepcache=True --no-docs install \
      pvs-studio \
&& \
  printf '%s\n' \
         "[viva64-beta]" \
	 "name=Viva64 - PVS-Studio - Beta" \
         "baseurl=http://cdn.pvs-studio.com/beta/rpm" \
	 "gpgcheck=0" \
	 "enabled=1" \
         "skip_if_unavailable=True" > /etc/yum.repos.d/viva64-beta.repo && \
  dnf -q -y --setopt=install_weak_deps=True --setopt=keepcache=True --no-docs upgrade \
&& \
  mkdir -p /tmp/duma-build && \
  cd /tmp/duma-build && \
  git clone -v https://github.com/johnsonjh/duma && \
  cd duma && \
  make && \
  make install && \
  cd / && \
  rm -rf /tmp/duma-build && \
  ldconfig \
&& \
  dnf -q -y clean all && \
  rm -rf ~/.cache && \
  rm -rf /var/tmp/* && \
  rm -rf /var/log/dnf* && \
  ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true) \
&& \
  rm -f /etc/crypto-policies/back-ends/opensslcnf.config && \
  printf '%s\n' \
    "CipherString = @SECLEVEL=2:kEECDH:kRSA:kEDH:kPSK:kDHEPSK:kECDHEPSK:kRSAPSK:-aDSS:-3DES:!DES:!RC4:!RC2:!IDEA:-SEED:!eNULL:!aNULL:!MD5:-SHA384:-CAMELLIA:-ARIA:-AESCCM8" \
    "Ciphersuites = TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256:TLS_AES_128_CCM_SHA256" \
    "SignatureAlgorithms = ECDSA+SHA256:ECDSA+SHA384:ECDSA+SHA512:ed25519:ed448:rsa_pss_pss_sha256:rsa_pss_pss_sha384:rsa_pss_pss_sha512:rsa_pss_rsae_sha256:rsa_pss_rsae_sha384:rsa_pss_rsae_sha512:RSA+SHA256:RSA+SHA384:RSA+SHA512:ECDSA+SHA224:RSA+SHA224" \
    "" \
    "[openssl_init]" \
    "" \
    "[evp_properties]" \
    "" \
&& \
  touch /.OK
