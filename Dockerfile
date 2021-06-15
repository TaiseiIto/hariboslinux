FROM alpine
RUN apk update

# gcc, git, ld, make, etc.
RUN apk add --no-cache alpine-sdk
# emulator
RUN apk add --no-cache qemu-system-i386

# clone the repository
WORKDIR ~
RUN git clone https://github.com/TaiseiIto/hariboslinux.git

# make the OS image file
WORKDIR hariboslinux
RUN make

# update the OS image file
CMD git pull origin master
CMD make

