FROM debian:stable-slim

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y build-essential git cmake libhidapi-dev wget dpkg tar gzip vim

RUN cd /home && \
    mkdir conan-deb &&\
    cd conan-deb &&\
    wget https://github.com/conan-io/conan/releases/download/1.61.0/conan-ubuntu-64.deb && \
    dpkg -i conan-ubuntu-64.deb && \
    rm -r -f ../conan-deb

RUN apt-get clean -y

# copy conan files
COPY conan/ /root/.conan/
# copy conan profiles
COPY conan_profiles/linux/centos7/ /root/.conan/profiles
# private.pem
COPY private.pem /opt

RUN cd /home && \
    mkdir project && \
    cd project &&\
    mkdir scanner

COPY conanfile.txt /home/project/scanner

VOLUME /home/bobrov/project/scanner /home/project/scanner

WORKDIR /home/project/scanner

RUN cd /home/project/scanner && mkdir build_debian_x86_64 && \
    conan install -pr linux_x86_64_build -if build_debian_x86_64 /home/project/scanner/ --build=missing 