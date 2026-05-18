# Use a stable Ubuntu base
FROM ubuntu:latest

# Set a working directory
WORKDIR /work

# Install basic tools and dependencies
RUN apt-get update && apt-get install -y \
    git \
    build-essential \
    cmake \
    ca-certificates \
    libuhd-dev \
    nlohmann-json3-dev \
    python3-pybind11 pybind11-dev \
    libboost-program-options-dev && rm -rf /var/lib/apt/lists/* 

ADD . "mantis"

RUN if [ -d "mantis/build" ]; then rm -rf mantis/build; fi

RUN cd mantis && mkdir -p build \
&& cd build && cmake .. && make -j $(nproc) install \
&& cd ../.. && rm -rf mantis

RUN ldconfig

# Default shell when container starts
ENTRYPOINT ["/bin/bash"]
