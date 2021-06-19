FROM alpine
RUN apk update

# gcc, git, ld, make, etc.
RUN apk add --no-cache alpine-sdk
# emulator
RUN apk add --no-cache qemu-system-i386
# solve the PID 1 problem
RUN apk add --no-cache tini
# editor
RUN apk add --no-cache vim

# clone the repository
WORKDIR ~
RUN git clone https://github.com/TaiseiIto/hariboslinux.git

# make the OS image file
WORKDIR hariboslinux
RUN make

# solve the PID 1 problem
ENTRYPOINT /sbin/tini -e 143 -- /bin/sh

# VNC port
EXPOSE 5900

