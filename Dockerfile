FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y numactl git cmake software-properties-common build-essential libtool autoconf unzip wget g++ python3 python3-pip libomp-dev libhwloc-dev libjemalloc-dev patchelf elfutils pkg-config && \
    pip3 install matplotlib numpy seaborn pandas && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*
COPY . /sources
RUN cd sources && git submodule update --init && chmod +x run-me.sh
WORKDIR /sources
