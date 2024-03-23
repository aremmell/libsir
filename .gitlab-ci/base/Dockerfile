################################################################################
#
# Version: 2.2.5
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2024 Jeffrey H. Johnson <trnsz@pobox.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
################################################################################

FROM quay.io/fedora/fedora:rawhide

USER 0

WORKDIR /

ARG URL

RUN set -x && \
  (sed -i 's/tsflags=nodocs//' /etc/dnf/dnf.conf || true) && \
  dnf -y --refresh update && \
  dnf -y install dnf-plugins-core dnf-utils && \
  dnf -y install -y 'dnf-command(config-manager)' && \
  dnf -y --refresh update && \
  dnf -y install rpmconf && \
  (rpmconf --all --unattended use_maintainer || true) && \
  (rpmconf --all --unattended use_maintainer --clean || true) && \
  dnf -y --refresh update && \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      harfbuzz.i686 \
      harfbuzz.x86_64 \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      advancecomp \
      ansifilter \
      atool \
      bear \
      boost \
      boost-devel \
      bzip2 \
      ca-certificates \
      ccache \
      ccache-swig \
      clang \
      clang-analyzer \
      clang-tools-extra \
      cmake \
      compiler-rt.i686 \
      compiler-rt.x86_64 \
      cppcheck \
      cppcheck-htmlreport \
      cppi \
      croc \
      curl \
      doxygen \
      expect \
      fakeroot \
      file \
      flawfinder \
      fmt \
      fmt-devel \
      fontconfig.i686 \
      fontconfig.x86_64 \
      g++ \
      gawk \
      gcc \
      gcc-gm2 \
      gcovr \
      git \
      glibc-devel \
      glibc-devel.i686 \
      glibc-devel.x86_64 \
      glibc-headers-x86 \
      glibc-static \
      global \
      golang \
      guile \
      guile-devel \
      libasan \
      libasan.i686 \
      libasan.x86_64 \
      libnsl \
      libnsl2 \
      libnsl2.i686 \
      libnsl2.x86_64 \
      libnsl.i686 \
      libnsl.x86_64 \
      libtirpc \
      libtirpc.i686 \
      libtirpc.x86_64 \
      libubsan \
      libubsan.i686 \
      libubsan.x86_64 \
      libxml2-devel \
      libxslt-devel \
      lld \
      lua \
      lua-devel \
      lzip \
      make \
      musl-devel \
      musl-gcc \
      musl-libc \
      musl-libc-static \
      octave \
      openssl \
      pbzip2 \
      perl \
      perl-core \
      perl-devel \
      pigz \
      procps-ng \
      python3 \
      python3-devel \
      python3-setuptools \
      python-setuptools \
      reuse \
      ruby \
      ruby-devel \
      screen \
      swig \
      symlinks \
      tcl \
      tcl-devel \
      time \
      tmux \
      unzip \
      valgrind \
      vim \
      wget \
      yasm \
      zip \
      zpaq \
&& \
  curl -fsSL https://rpm.nodesource.com/setup_21.x | bash - && \
  dnf install -y --setopt=install_weak_deps=True --setopt=keepcache=True nodejs && \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True \
    --skip-broken --best --allowerasing group install \
      development-tools \
      c-development \
&& \
  dnf -y --refresh update && \
  (rpmconf --all --unattended use_maintainer || true) && \
  (rpmconf --all --unattended use_maintainer --clean || true) && \
  (curl -fsSL https://coveralls.io/coveralls-linux.tar.gz | tar -zxv -C /usr/bin) && \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      mingw32-binutils \
      mingw32-boost \
      mingw32-boost-static \
      mingw32-cpp \
      mingw32-crt \
      mingw32-dlfcn \
      mingw32-dlfcn-static \
      mingw32-gcc \
      mingw32-gcc-c++ \
      mingw32-headers \
      mingw32-win-iconv \
      mingw32-winpthreads \
      mingw32-winpthreads-static \
      mingw64-binutils \
      mingw64-boost \
      mingw64-boost-static \
      mingw64-cpp \
      mingw64-crt \
      mingw64-dlfcn \
      mingw64-dlfcn-static \
      mingw64-gcc \
      mingw64-gcc-c++ \
      mingw64-headers \
      mingw64-win-iconv \
      mingw64-winpthreads \
      mingw64-winpthreads-static \
      mingw-binutils-generic \
      mingw-w64-tools \
      ucrt64-binutils \
      ucrt64-cpp \
      ucrt64-crt \
      ucrt64-gcc \
      ucrt64-gcc-c++ \
      ucrt64-headers \
      ucrt64-winpthreads \
      ucrt64-winpthreads-static \
&& \
  (yes | dnf -y copr enable yselkowitz/cygwin fedora-rawhide-x86_64) && \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
      cygport \
      cygwin32 \
      cygwin32-binutils \
      cygwin32-gcc \
      cygwin64 \
      cygwin64-binutils \
      cygwin64-gcc-c++ \
      cygwin-w32api-headers \
&& \
  dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install wine && \
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
  cd / && \
  (dnf -y --setopt=install_weak_deps=True --setopt=keepcache=True install \
     --nogpgcheck ${URL:-http://192.168.1.187:8088/~jhj/cache/aocc-compiler.x86_64.rpm}) && \
  mkdir -p /tmp/duma-build && \
  cd /tmp/duma-build && \
  git clone -v https://github.com/johnsonjh/duma && \
  cd duma && \
  make && \
  make install && \
  cd / && \
  rm -rf /tmp/duma-build && \
  ldconfig && \
  dnf -y clean all && \
  rm -rf ~/.cache && \
  rm -rf /var/tmp/* && \
  rm -rf /var/log/dnf* && \
  rm -f /etc/crypto-policies/back-ends/opensslcnf.config && \
  printf '%s\n' \
    "CipherString = @SECLEVEL=2:kEECDH:kRSA:kEDH:kPSK:kDHEPSK:kECDHEPSK:kRSAPSK:-aDSS:-3DES:!DES:!RC4:!RC2:!IDEA:-SEED:!eNULL:!aNULL:!MD5:-SHA384:-CAMELLIA:-ARIA:-AESCCM8" \
    "Ciphersuites = TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256:TLS_AES_128_CCM_SHA256" \
    "SignatureAlgorithms = ECDSA+SHA256:ECDSA+SHA384:ECDSA+SHA512:ed25519:ed448:rsa_pss_pss_sha256:rsa_pss_pss_sha384:rsa_pss_pss_sha512:rsa_pss_rsae_sha256:rsa_pss_rsae_sha384:rsa_pss_rsae_sha512:RSA+SHA256:RSA+SHA384:RSA+SHA512:ECDSA+SHA224:RSA+SHA224" \
    "" \
    "[openssl_init]" \
    "" \
    "[evp_properties]" \
    "" > /etc/crypto-policies/back-ends/opensslcnf.config && \
    dnf -y clean all && \
    rm -rf ~/.cache > /dev/null 2>&1 && \
    rm -rf /var/tmp/* > /dev/null 2>&1 && \
    rm -rf /var/log/dnf* > /dev/null 2>&1 && \
    adduser -m -U -G wheel --uid 10000 -c "CI/CD User" user && \
    touch /.OK

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
