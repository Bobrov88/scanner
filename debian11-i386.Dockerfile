FROM i386/debian:bullseye-slim

RUN apt-get update -y && \
    apt-get upgrade -y 

RUN apt-get clean -y