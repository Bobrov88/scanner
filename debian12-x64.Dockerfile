FROM debian:stable-slim

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y build-essential git cmake libhidapi-dev libudev-dev libusb-1.0 wget dpkg tar gzip vim pkg-config

RUN apt-get clean -y

RUN cd /home && \
    mkdir conan-deb &&\
    cd conan-deb &&\
    wget https://github.com/conan-io/conan/releases/download/1.61.0/conan-ubuntu-64.deb && \
    dpkg -i conan-ubuntu-64.deb && \
    rm -r -f ../conan-deb


# copy conan files
COPY conan/ /root/.conan/
# copy conan profiles
COPY conan_profiles/linux/debian12/ /root/.conan/profiles
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

#docker run --rm -it --volume=./:/home/project/scanner scanner_x64
#cmake -Bbuild_debian_x86_64 . -DTARGET_SYSTEM=DEBIAN -D_DEBUG=TRUE
#cmake --build ./build_debian_x86_64