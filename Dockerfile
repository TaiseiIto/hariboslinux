FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y && apt-get install build-essential -y
RUN apt-get update && apt-get upgrade -y && apt-get install bison -y
RUN apt-get update && apt-get upgrade -y && apt-get install dejagnu -y
RUN apt-get update && apt-get upgrade -y && apt-get install libexpat-dev -y
RUN apt-get update && apt-get upgrade -y && apt-get install flex -y
RUN apt-get update && apt-get upgrade -y && apt-get install git -y
RUN apt-get update && apt-get upgrade -y && apt-get install git-email -y
RUN apt-get update && apt-get upgrade -y && apt-get install libglib2.0-dev -y
RUN apt-get update && apt-get upgrade -y && apt-get install libgmp-dev -y
RUN apt-get update && apt-get upgrade -y && apt-get install libmpfr-dev -y
RUN apt-get update && apt-get upgrade -y && apt-get install iasl -y
RUN apt-get update && apt-get upgrade -y && apt-get install libpixman-1-dev -y
RUN apt-get update && apt-get upgrade -y && apt-get install ninja-build -y
RUN apt-get update && apt-get upgrade -y && apt-get install python3 -y
RUN apt-get update && apt-get upgrade -y && apt-get install pkg-config -y
RUN apt-get update && apt-get upgrade -y && apt-get install texinfo -y
RUN apt-get update && apt-get upgrade -y && apt-get install tmux -y
RUN apt-get update && apt-get upgrade -y && apt-get install tzdata -y
RUN apt-get update && apt-get upgrade -y && apt-get install vim -y
RUN apt-get update && apt-get upgrade -y && apt-get install wget -y

# Put git settings.
WORKDIR /root
RUN mkdir .ssh
RUN mkdir .github

# Set local time.
RUN cp /usr/share/zoneinfo/Asia/Tokyo /etc/localtime

# GDB
WORKDIR /root
RUN git clone --branch gdb-14.1-release --depth 1 --recursive --shallow-submodules --single-branch https://sourceware.org/git/binutils-gdb.git
WORKDIR /root/binutils-gdb
RUN ./configure --with-expat CFLAGS="-O0 -g -fno-inline" CXXFLAGS="-O0 -g -fno-inline"
RUN make
RUN make install

# QEMU
WORKDIR /root
RUN git clone --branch v8.2.0 --depth 1 --recursive --shallow-submodules --single-branch https://gitlab.com/qemu-project/qemu.git
WORKDIR /root/qemu
RUN ./configure --target-list=i386-softmmu CFLAGS="-O0 -g -fno-inline" CXXFLAGS="-O0 -g -fno-inline"
RUN make
RUN make install

# hariboslinux
WORKDIR /root
RUN git clone https://github.com/TaiseiIto/hariboslinux.git
WORKDIR /root/hariboslinux
# RUN make

# ash setting
RUN cat ash/.profile >> /root/.bashrc

# gdb setting
RUN echo add-auto-load-safe-path `pwd`/gdb/.gdbinit > /root/.gdbinit
RUN echo set print elements 0 >> /root/.gdbinit

# gdb real mode disassemble
RUN wget https://raw.githubusercontent.com/qemu/qemu/master/gdb-xml/i386-32bit.xml -P gdb

# tmux setting
RUN cp tmux/.tmux.conf ..

# vim setting
RUN cat vim/.vimrc >> ../.vimrc

# VNC port
EXPOSE 5900

