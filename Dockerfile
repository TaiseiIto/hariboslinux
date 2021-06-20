FROM alpine
RUN apk update

# gcc, git, ld, make, etc.
RUN apk add --no-cache alpine-sdk
# emulator
RUN apk add --no-cache qemu-system-i386
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

# VNC port
EXPOSE 5900

