FROM ubuntu

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
# gcc, ld, make, etc.
RUN apt install build-essential -y
# bison
RUN apt install bison -y
# dejagnu
RUN apt install dejagnu -y
# expat
RUN apt install libexpat-dev -y
# flex
RUN apt install flex -y
# git
RUN apt install git -y
RUN apt install git-email -y
RUN git config --global pull.rebase false
RUN mkdir /root/.ssh
# glib
RUN apt install libglib2.0-dev -y
# gmp
RUN apt install libgmp-dev -y
# pixman
RUN apt install libpixman-1-dev -y
# ninja
RUN apt install ninja-build -y
# python
RUN apt install python3 -y
# pkg-config
RUN apt install pkg-config -y
# texinfo
RUN apt install texinfo -y
# tmux
RUN apt install tmux -y
# tzdata
RUN apt install tzdata -y
RUN cp /usr/share/zoneinfo/Asia/Tokyo /etc/localtime
# editor
RUN apt install vim -y
# wget
RUN apt install wget -y

# GDB
WORKDIR /root
RUN git clone git://sourceware.org/git/binutils-gdb.git
WORKDIR /root/binutils-gdb
RUN ./configure --with-expat CFLAGS="-O0 -g -fno-inline" CXXFLAGS="-O0 -g -fno-inline"
RUN make
RUN make install

# QEMU
WORKDIR /root
RUN git clone https://gitlab.com/qemu-project/qemu.git
WORKDIR /root/qemu
RUN ./configure --target-list=i386-softmmu CFLAGS="-O0 -g -fno-inline" CXXFLAGS="-O0 -g -fno-inline"
RUN make
RUN make install

# hariboslinux
WORKDIR /root
RUN git clone https://github.com/TaiseiIto/hariboslinux.git
WORKDIR /root/hariboslinux
RUN git checkout issue105
RUN make

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

