FROM alpine
RUN apk update

# gcc, git, ld, make, etc.
RUN apk add --no-cache alpine-sdk
# emulator
RUN apk add --no-cache qemu-system-i386

# clone the repository
WORKDIR ~
RUN git clone https://github.com/TaiseiIto/hariboslinux.git

# make a image file of haribos floppy disk
WORKDIR hariboslinux
RUN make

CMD pwd

