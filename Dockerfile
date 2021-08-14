FROM alpine
RUN apk update

# gcc, git, ld, make, etc.
RUN apk add --no-cache alpine-sdk
# git setting
RUN git config --global pull.rebase false
# debugger
RUN apk add --no-cache gdb
# emulator
RUN apk add --no-cache qemu-system-i386
# ssh
RUN apk add openssh
# editor
RUN apk add --no-cache vim
# set time zone UTC+9 (Japan)
RUN apk add --no-cache tzdata
RUN cp /usr/share/zoneinfo/Asia/Tokyo /etc/localtime
RUN apk del tzdata

# clone the repository
WORKDIR ~
RUN git clone https://github.com/TaiseiIto/hariboslinux.git

# make the OS image file
WORKDIR hariboslinux
RUN make

# gdb setting
RUN echo add-auto-load-safe-path `pwd`/gdb/.gdbinit > ~/.gdbinit

# gdb real mode disassemble
RUN wget https://raw.githubusercontent.com/qemu/qemu/master/gdb-xml/i386-32bit.xml -P gdb

# VNC port
EXPOSE 5900

