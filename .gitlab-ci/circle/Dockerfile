# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Ryan M. Lederman

# Use --build-arg URL="..." to override default

FROM registry.gitlab.com/libsir/libsir/base:latest

USER 0

WORKDIR /

ARG URL

RUN dnf -y install \
              pigz \
              sharutils \
              xdelta && \
    ( dnf4 -y clean > /dev/null 2>&1 ; dnf4 -y clean all > /dev/null 2>&1 ; true ) || true && \
    ( dnf4 -y clean all || true ) && \
    cd /tmp && mkdir -p circle && cd circle && \
    ( wget -S -v -c -t 0 https://www.circle-lang.org/linux/build_200.tgz || \
      wget -S -v -c -t 0 ${URL:-http://192.168.1.187:8088/~jhj/cache/build_200.tgz} ) && \
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
    ((symlinks -r -d /usr 2>&1 | tee /dev/stderr | grep dangling) || true) && \
    touch /OK

USER 10000
