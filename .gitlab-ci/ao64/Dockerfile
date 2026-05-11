################################################################################
#
# Version: 2.2.6
#
################################################################################
#
# SPDX-License-Identifier: MIT
#
# Copyright (c) 2018-2026 Jeffrey H. Johnson <johnsonjh.dev@gmail.com>
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

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN set -x && \
      wget "${URL:-http://172.17.0.1:8088/x86_open64-4.5.2.1-1.x86_64.rpm}" && \
      rpm -ivh --nodigest --nofiledigest x86_open64-4.5.2.1-1.x86_64.rpm && \
      rm -f x86_open64-4.5.2.1-1.x86_64.rpm && \
      printf '%s\n' 'int matherr=0;' 'char* _LIB_VERSION="";' > tmp.c && \
      gcc -o libmcompat.o -c tmp.c && \
      ar -crs /usr/lib64/libmcompat.a libmcompat.o && \
      rm -f libmcompat.o && \
      gcc -m32 -o libmcompat.o -c tmp.c && \
      ar -crs /usr/lib/libmcompat.a libmcompat.o && \
      rm -f libmcompat.o && \
      rm -f tmp.c && \
      ranlib /usr/lib64/libmcompat.a && \
      ranlib /usr/lib/libmcompat.a && \
      ldconfig -v && \
      dnf -y clean all && \
      rm -rf ~/.cache > /dev/null 2>&1 && \
      rm -rf /var/tmp/* > /dev/null 2>&1 && \
      rm -rf /var/log/dnf* > /dev/null 2>&1

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
