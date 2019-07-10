FROM ubuntu:19.04

RUN apt-get update && apt-get install -y wget git ssh tar gzip ca-certificates
RUN apt-get install -y gnupg
RUN echo "deb http://apt.llvm.org/disco/ llvm-toolchain-disco-8 main" >> /etc/apt/sources.list
RUN echo "deb-src http://apt.llvm.org/disco/ llvm-toolchain-disco-8 main" >> /etc/apt/sources.list

RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-get update && apt-get install -y llvm libreadline-dev cmake clang

