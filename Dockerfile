FROM ubuntu:jammy

RUN apt update && \
	apt upgrade -y && \
	DEBIAN_FRONTEND=noninteractive apt install \
		build-essential \
		gcc-9 \
		clang-13 \
		clang-tidy-13 \
		llvm-13-dev \
		shellcheck \
		git \
		file -y

RUN sed -i "s/LLVMCreatePassBuilderOptions()/LLVMCreatePassBuilderOptions(void)/" $(llvm-config-13 --includedir)/llvm-c/Transforms/PassBuilder.h

WORKDIR /app
COPY --chown=1000 . .
RUN chown 1000 .
USER 1000

RUN clang-tidy-13 --version
RUN make lint

RUN make TRACE=1 CC=gcc-9 options clean all
RUN ./build/test/test
RUN ./test/sh/main.sh
RUN make clean

RUN make TRACE=1 CC=clang-13 options clean all
RUN ./build/test/test
RUN ./test/sh/main.sh
RUN make clean

USER 0
RUN make TRACE=1 CC=gcc-9 options install install-dev
USER 1000
RUN skull test/main.sk
RUN ./test/main

RUN cd test/docs/c-integration && make
