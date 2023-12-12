FROM debian:stable-slim

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y build-essential git cmake libhidapi-dev wget dpkg

RUN cd /home && \
    mkdir conan-deb &&\
    cd conan-deb &&\
    wget https://github.com/conan-io/conan/releases/download/1.61.0/conan-ubuntu-64.deb && \
    dpkg -i conan-ubuntu-64.deb && \
    rm -r -f ../conan-deb

RUN apt-get clean -y

RUN cd /home && \
    mkdir project && \
    cd project &&\
    mkdir scanner
WORKDIR /home/project/scanner