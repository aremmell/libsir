# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

## NOTE: Uses local webserver at 192.168.1.187:8088

FROM quay.io/fedora/fedora:rawhide

USER 0

WORKDIR /

RUN \
  (sed -i 's/tsflags=nodocs//' /etc/dnf/dnf.conf || true) \
&& \
  dnf -y --refresh update \
&& \
  dnf -y install dnf-plugins-core dnf-utils \
&& \
  dnf -y --refresh update \
&& \
  dnf -y install rpmconf \
&& \
  (rpmconf --all --unattended use_maintainer || true) \
&& \
  (rpmconf --all --unattended use_maintainer --clean || true) \
&& \
  dnf -y --refresh update \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      advancecomp \
      ansifilter \
      atool \
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
      gcovr \
      git \
      glibc-static \
      global \
      libnsl \
      libnsl2 \
      libnsl2.i686 \
      libnsl.i686 \
      libtirpc \
      libtirpc.i686 \
      libxml2-devel \
      libxslt-devel \
      lld \
      make \
      musl-devel \
      musl-gcc \
      musl-libc \
      musl-libc-static \
      openssl \
      perl-core \
      pigz \
      python3 \
      python3-devel \
      python3-setuptools \
      reuse \
      screen \
      symlinks \
      time \
      tmux \
      unzip \
      valgrind \
      vim \
      wget \
      zip \
&& \
  ( curl -fsSLO https://rpm.nodesource.com/setup_20.x || true ) \
&& \
  ( chmod a+x "setup_20.x" || true ) \
&& \
  ( sed -i "s/## Given the distro.*/DIST_VERSION=38/" "setup_20.x" || true ) \
&& \
  ( bash -x ./setup_20.x || true ) \
&& \
  dnf -y --refresh update \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      nodejs \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True group install \
      development-tools \
      c-development \
&& \
  dnf -y --refresh update \
&& \
  (rpmconf --all --unattended use_maintainer || true) \
&& \
  (rpmconf --all --unattended use_maintainer --clean || true) \
&& \
  (curl -fsSL https://coveralls.io/coveralls-linux.tar.gz | tar -zxv -C /usr/bin) \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
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
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      cygport \
      cygwin32 \
      cygwin32-binutils \
      cygwin32-gcc \
      cygwin64 \
      cygwin64-binutils \
      cygwin64-gcc \
      cygwin-w32api-headers \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install wine \
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
  (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          pvs-studio) || \
  (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          --nogpgcheck http://192.168.1.187:8088/~jhj/cache/pvs-studio.x86_64.rpm) \
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
  (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
          --nogpgcheck http://192.168.1.187:8088/~jhj/cache/aocc-compiler.x86_64.rpm) \
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
  dnf -y clean all && \
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
    "" > /etc/crypto-policies/back-ends/opensslcnf.config \
&& \
  touch /.OK

RUN adduser -m -U -G wheel --uid 10000 -c "CI/CD User" user

RUN git clone -v https://github.com/Ericsson/codechecker.git --depth=1 && \
    cd codechecker && \
    ( grep 'lxml==4\.9\.2' -rl . | xargs -I{} sed -i 's/lxml==4.9.2/lxml==4.9.3/' "{}" || true ) && \
    make venv && \
    . $(pwd)/venv/bin/activate && \
    pip install setuptools && \
    env BUILD_LOGGER_64_BIT_ONLY=YES \
                   BUILD_UI_DIST=NO \
	make package standalone_package && \
    export PATH=/codechecker/build/CodeChecker/bin:$PATH && \
    chown user:user -R /codechecker

USER 10000
