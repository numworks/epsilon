# Dockerfile used by the CI

FROM debian:bookworm-slim

WORKDIR /runner

RUN echo 'APT::Get::Assume-Yes "true";' > /etc/apt/apt.conf.d/assume-yes && \
    apt-get update && \
    apt-get upgrade && \
    apt-get install --no-install-recommends sudo

COPY build/setup.sh build/setup.sh

RUN build/setup.sh --ci

ENV PYTHON=/runner/.venv/bin/python

WORKDIR /epsilon
