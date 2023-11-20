# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Jeffrey H. Johnson <trnsz@pobox.com>

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN dnf --refresh -y upgrade && dnf --nogpgcheck -y install \
      ${URL:-http://192.168.1.187:8088/~jhj/cache/x86_open64-4.5.2.1-1.x86_64.rpm} && \
      printf '%s\n' 'int matherr=0;' 'char* _LIB_VERSION="";' > tmp.c && \
      gcc -o libmcompat.o -c tmp.c && \
      ar -crs /usr/lib64/libmcompat.a libmcompat.o && \
      env rm -f libmcompat.o && \
      env rm -f tmp.c && \
      ranlib /usr/lib64/libmcompat.a && \
      ldconfig -v && \
      dnf -y clean all && \
      rm -rf ~/.cache > /dev/null 2>&1 && \
      rm -rf /var/tmp/* > /dev/null 2>&1 && \
      rm -rf /var/log/dnf* > /dev/null 2>&1 && \
      ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true)

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
