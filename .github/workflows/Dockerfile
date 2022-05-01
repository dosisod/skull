FROM skull:ubuntu-base

WORKDIR /app
COPY --chown=1000 . .
RUN chown 1000 .
USER 1000

RUN make TRACE=1 CC=gcc-11 options clean all
RUN ./build/test/test
RUN ./test/sh/main.sh
RUN make clean

RUN make TRACE=1 CC=clang-13 options clean all
RUN ./build/test/test
RUN ./test/sh/main.sh
RUN make clean

USER 0
RUN make TRACE=1 CC=gcc-11 options install install-dev
USER 1000
RUN skull test/main.sk
RUN ./test/main

USER 0
RUN make CC=gcc-11 clean install install-dev
USER 1000
RUN cc test/libskull.c -o skull-shim && ./skull-shim

RUN cd test/docs/c-integration && make

RUN clang-tidy-13 --version
RUN make lint
RUN shellcheck test/sh/main.sh Makefile
