FROM i386/debian:11

RUN apt-get update -y && \
    apt-get upgrade -y && \
    apt-get install -y && \
        build-essential \
        git \
        libhidapi-dev \
        libudev-dev \
        libusb-1.0 \
        wget \
        dpkg \
        tar \
        gzip \
        vim \
        curl \
        pkg-config \
        wget \
        libreadline-dev \
        libncursesw5-dev \
        libssl-dev \
        libsqlite3-dev \
        tk-dev \
        libgdbm-dev \
        libc6-dev \
        libbz2-dev \
        libffi-dev \
        zlib1g-dev

RUN apt-get clean -y

RUN cd / && \
    PYTHON_VERSION=3.8.12 && \
    wget https://mirrors.huaweicloud.com/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz && \
    #wget https://www.python.org/ftp/python/${PYTHON_VERSION}/Python-${PYTHON_VERSION}.tgz && \ \
    #wget https://github.com/Bobrov88/Old_packages/blob/master/Python-${PYTHON_VERSION}.tgz && \ \
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
    pip install --force-reinstall -v "conan==1.64.0" && \
    pip install -U markdown && \
    conan config set general.revisions_enabled=1 && \
    cd / && rm -rf Python-${PYTHON_VERSION}.tgz && rm -rf Python-${PYTHON_VERSION}

RUN CMAKE_VERSION=3.20.0 && \
    wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz && \
    tar -zxvf cmake-${CMAKE_VERSION}.tar.gz && \
    cd cmake-${CMAKE_VERSION} && \
    ./bootstrap && \
    make && \
    make install && \
    rm -r -f ../cmake-${CMAKE_VERSION}

# copy conan files
COPY conan/ /root/.conan/
# copy conan profiles
COPY conan_profiles/linux/debian11/ /root/.conan/profiles
# private.pem
COPY private.pem /opt

RUN cd /home && \
    mkdir project && \
    cd project && \
    mkdir scanner

COPY conanfile.txt /home/project/scanner

VOLUME /home/bobrov/project/scanner /home/project/scanner

RUN cd /home/project/scanner && mkdir build_debian11_x86 && \
    conan install -pr linux_x86_build -if build_debian11_x86 /home/project/scanner/ --build=missing

WORKDIR /home/project/scanner
