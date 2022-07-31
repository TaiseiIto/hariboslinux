FROM ubuntu
RUN apt update

# gcc, git, ld, make, etc.
RUN apt install build-essential -y
# bison
RUN apt install bison -y
# expat
RUN apt install libexpat-dev -y
# flex
RUN apt install flex -y
# git
RUN apt install git -y
RUN git config --global pull.rebase false
RUN mkdir /root/.ssh
# gmp
RUN apt install libgmp-dev -y
# emulator
RUN apt install qemu-system-i386 -y
# texinfo
RUN apt install texinfo -y
# tmux
RUN apt install tmux -y
# editor
RUN apt install vim -y
# wget
RUN apt install wget -y

# clone GDB
WORKDIR /root
RUN git clone git://sourceware.org/git/binutils-gdb.git
WORKDIR /root/binutils-gdb
RUN ./configure --with-expat
RUN make install

# clone the repository
WORKDIR /root
RUN git clone https://github.com/TaiseiIto/hariboslinux.git
WORKDIR /root/hariboslinux
RUN make

# ash setting
RUN cat ash/.profile >> /root/.bashrc

# gdb setting
RUN echo add-auto-load-safe-path `pwd`/gdb/.gdbinit > /root/.gdbinit

# gdb real mode disassemble
RUN wget https://raw.githubusercontent.com/qemu/qemu/master/gdb-xml/i386-32bit.xml -P gdb

# tmux setting
RUN cp tmux/.tmux.conf ..

# vim setting
RUN cat vim/.vimrc >> ../.vimrc

# VNC port
EXPOSE 5900

