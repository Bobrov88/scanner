FROM debian:experimental

RUN apt update -y && \
    apt upgrade -y

RUN apt update -y && \
    apt install -y build-essential

RUN apt install cmake -yum

RUN apt install python3-pip -y

RUN pip install conan==1.61.0

WORKDIR /home/project/scanner