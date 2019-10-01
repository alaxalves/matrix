FROM debian:stretch-slim

# Installing OpenMP and MPI
RUN apt update && \
    apt install -y build-essential \
                   libomp-dev \
                   openmpi-bin \
                   openssh-client \
                   openssh-server \
                   libopenmpi-dev

COPY . /matrix

WORKDIR /matrix

ENV PROCESSORS=1

RUN make
ENTRYPOINT ["make", "run"] 
