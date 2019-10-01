FROM debian:stretch-slim

# Installing OpenMP and MPI
RUN apt update && \
    apt install -y build-essential \
                   libomp-dev \
                   openmpi-bin \
                   openssh-client \
                   openssh-server \
                   libopenmpi-dev