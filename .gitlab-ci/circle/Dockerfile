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

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN set -x && \
    dnf -y install \
              pigz \
              sharutils \
              xdelta && \
    cd /tmp && mkdir -p circle && cd circle && \
    ( wget -q https://www.circle-lang.org/linux/build_200.tgz || \
      wget -q "${URL:-http://192.168.1.187:8088/~jhj/cache/build_200.tgz}" ) && \
    pigz -dvc build_200.tgz | tar xvf - circle && \
    printf '%s\n' \
      'begin 644 circle200.vcdiff'                                        \
      'MUL/$``4"#V-I<F-L92''''\O8VER8VQE+P6$@(```!*$@(`````%`0@#_XP3A("`' \
      'M```%A("``(2`@``2A(''''"`````!0%X.JO/$X2`@```!82`@`"(@(``$H2`@```' \
      'M``4!O/C:\1.$@(````''''6$@(``C("``!*$@(`````%`1Z3%7$3A("````%A("`' \
      'M`)"`@``2A("`````!0'"'"'V4O9;$X2`@```!82`@`"4@(``9X2`@``&!3`F!/$[' \
      'M[B`O+R\O%/TW>EA:``''''``_Q+900(`(0$,````CYA!G`$`$Q.2M!"P$QHF4X-#' \
      'M`S-<`T.#[<<G"OTW>E''''A:````_Q+900(`(0$,````CYA!G`$`"0`!DK06`2"#' \
      'M15X%A("``)B`@``2A(''''"`````!0%6T!<W$X2`@```!82`@`"<@(``$H2`@```' \
      'M``4!DS?DDQ.$@(````''''6$@(``H("``!*$@(`````%`1:@I_,3A("````%@9G"' \
      '8`*2`@``2@9G"````!0''''&-(U^_$X&9P@``'                             \
      '`'                                                                 \
      'end' | uudecode && \
    xdelta3 -d -s circle circle200.vcdiff /usr/bin/circle && \
    chmod a+x /usr/bin/circle && \
    strip --strip-all /usr/bin/circle && \
    cd .. && env rm -rf ./circle && cd && \
    dnf -y clean all && \
    rm -rf ~/.cache && \
    rm -rf /var/tmp/* && \
    rm -rf /var/log/dnf* && \
    touch /OK

USER 10000

RUN printf '\n%s\n' "BUILD SUCCESS"
