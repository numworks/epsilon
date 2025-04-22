# Dockerfile used by the CI

FROM debian:bookworm-slim

WORKDIR /runner

RUN echo 'APT::Get::Assume-Yes "true";' > /etc/apt/apt.conf.d/assume-yes && \
    apt-get update && \
    apt-get upgrade && \
    apt-get install --no-install-recommends sudo

COPY build/setup.sh build/setup.sh

RUN build/setup.sh --ci

# Uncomment if you want to build with TOOLCHAIN=host-clang
RUN apt-get install clang-15 libc++-15-dev libc++abi-15-dev && ln -s /usr/bin/clang-15 /usr/local/bin/clang && ln -s /usr/bin/clang++-15 /usr/local/bin/clang++

ENV PYTHON=/runner/.venv/bin/python

ARG ABSOLUTE_PATH

WORKDIR ${ABSOLUTE_PATH}/epsilon-internal/epsilon-internal
