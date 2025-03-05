# Dockerfile used by the CI

FROM debian:bookworm-slim

WORKDIR /runner

RUN echo 'APT::Get::Assume-Yes "true";' > /etc/apt/apt.conf.d/assume-yes && \
    apt-get update && \
    apt-get upgrade && \
    apt-get install --no-install-recommends ca-certificates git make npm python3 xz-utils

RUN git clone https://github.com/emscripten-core/emsdk.git

RUN ./emsdk/emsdk install 3.1.61 && ./emsdk/emsdk activate 3.1.61
ENV PATH=/runner/emsdk/upstream/emscripten/:$PATH

COPY nwlink-0.0.18.tgz .
RUN npm install -g ./nwlink-0.0.18.tgz

WORKDIR /epsilon

EXPOSE 8000

RUN echo "case \$1 in \n\
  build) \n\
    make output/web/app.nwb \n\
    ;; \n\
  serve) \n\
    make server \n\
    ;; \n\
esac" > /entrypoint.sh

ENTRYPOINT ["bash", "/entrypoint.sh"]
