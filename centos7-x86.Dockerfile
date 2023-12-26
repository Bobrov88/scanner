FROM i386/centos:7

ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN yum update -y && \
    yum upgrade -y && \
    yum groupinstall -y 'Development Tools' && \
    yum install -y wget && \
    yum install -y openssl-devel && \
    yum install -y sqlite-devel libffi-devel && \
    yum install -y xz-devel && \
    yum install -y xz-lzma-compat && \
    yum install -y bzip2-devel && \
    yum install -y libX11-devel
 #   yum install -y libudev-devel && \
 #   yum install boost boost-thread boost-devel -y
#yum install -y qt5-qtbase-devel && \
#yum install -y qt-devel qt-config && \
#yum-builddep -y qt5-qtbase-devel

#install and build gcc 9.4.0
RUN mkdir /build && \
    GCC_VERSION=9.4.0 && \
    cd /build && \
    wget https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.gz --no-check-certificate && \
    tar xzvf gcc-${GCC_VERSION}.tar.gz && \
    mkdir obj.gcc-${GCC_VERSION} && \
    cd gcc-${GCC_VERSION} && \
    ./contrib/download_prerequisites && \
    cd ../obj.gcc-${GCC_VERSION} && \
    ../gcc-${GCC_VERSION}/configure --disable-multilib --enable-languages=c,c++ --with-tune=generic --with-arch=x86-64 --build=i686-redhat-linux && \
    make -j $(nproc) && \
    make install && \
    export CXX=/usr/local/bin/i686-redhat-linux-g++ && \
    export CC=/usr/local/bin/i686-redhat-linux-gcc && \
    cp /build/obj.gcc-9.4.0/stage1-i686-redhat-linux/libstdc++-v3/src/.libs/libstdc++.so.6.0.28 /usr/lib && \
    rm -f /usr/lib/libstdc++.so.6 && \
    ln -s /usr/lib/libstdc++.so.6.0.28 /usr/lib/libstdc++.so.6

#install and build cmake
RUN cd /build/ && \
    git clone https://gitlab.kitware.com/cmake/cmake.git && \
    cd cmake && \
    git checkout v3.20.0 && \
    ./bootstrap && \
    make && \
    make install

RUN cd / && \
    PYTHON_VERSION=3.8.12 && \
    wget https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz && \
    tar -xzf Python-${PYTHON_VERSION}.tgz && \
    cd Python-${PYTHON_VERSION}/ && ./configure --enable-optimizations --enable-loadable-sqlite-extensions && make && make install && \
    cd .. && \
    cp /usr/local/bin/python3.8 /usr/bin/ && \
    rm -f /usr/bin/python3 && \
    ln -s /usr/bin/python3.8 /usr/bin/python3 && \
    curl "https://bootstrap.pypa.io/get-pip.py" -o "get-pip.py" && \
    python3 get-pip.py && \
    python3 -m pip install --upgrade pip && \
    pip install --upgrade setuptools && \
    pip install --force-reinstall -v "conan==1.61.0" && \
    pip install -U markdown && \
    conan config set general.revisions_enabled=1 && \
    cd / && rm -rf Python-${PYTHON_VERSION}.tgz && rm -rf Python-${PYTHON_VERSION}

# copy conan files
COPY conan/ /root/.conan/
# copy conan profiles
COPY conan_profiles/linux/centos7/ /root/.conan/profiles
# private.pem
COPY private.pem /opt

#clean build folder and cache
RUN cd / && \
    rm -rf /build && \
    yum clean all

#creating folder for project
RUN cd /home && \
    mkdir project && \
    cd project &&\
    mkdir scanner

RUN yum install libusbx-devel -y && yum install libusb-devel -y && yum install -y libudev-devel

COPY conanfile.txt /home/project/scanner

VOLUME /home/bobrov/project/scanner /home/project/scanner

WORKDIR /home/project/scanner

RUN cd /home/project/scanner && \
    conan install -pr linux_x86_build -if build /home/project/scanner/ --build=missing && \
    cd build && \
    cmake ..
#RUN conan profile update settings.compiler.libcxx=libstdc++11 default
