ASSEMBLY_FILES = haribos.s

# compiler
COMPILER = gcc
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_OUTPUT_OPTION = -o

# docker
DOCKER = docker
DOCKER_IMAGE_NAME = hariboslinux
DOCKER_IMAGE_TAG = latest
DOCKER_CONTAINER_NAME = hariboslinux
DOCKER_CONTAINER_SHELL = /bin/sh
DOCKER_VNC_PORT_OPTION = -p 5900:5900

# emulator
# emulate intel 386 processor
EMULATOR = qemu-system-i386
# a raw image of the floppy disk
EMULATOR_DRIVE_OPTION = -drive file=$(IMAGE_FILE),format=raw,if=floppy
# VESA VBE 2.0
EMULATOR_VIDEO_OPTION = -vga std
# virtual network computing for all ip address
EMULATOR_VNC_OPTION = -vnc :0

# floppy disk image file of the built operating system
IMAGE_FILE = haribos.img

# linker
LINKER = ld
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT = link.ld
LINKER_SCRIPT_OPTION = -T

# build the operating system
all: build

build: $(IMAGE_FILE)

clean:
	rm -f *.o *.img

docker-build:
	$(DOCKER) build -t $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_TAG) .

docker-login:
	$(DOCKER) exec -i -t $(DOCKER_CONTAINER_NAME) $(DOCKER_CONTAINER_SHELL)

docker-run:
	$(DOCKER) run --name $(DOCKER_CONTAINER_NAME) $(DOCKER_VNC_PORT_OPTION) -i -t $(DOCKER_IMAGE_NAME)

docker-start:
	$(DOCKER) start $(DOCKER_CONTAINER_NAME)

docker-stop:
	$(DOCKER) stop $(DOCKER_CONTAINER_NAME)

rebuild: clean
	make build

# run the operating system on QEMU
run: $(IMAGE_FILE)
	$(EMULATOR) $(EMULATOR_DRIVE_OPTION) $(EMULATOR_VIDEO_OPTION) $(EMULATOR_VNC_OPTION)

$(IMAGE_FILE): $(ASSEMBLY_FILES:.s=.o)
	$(LINKER) $^ $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(LINKER_SCRIPT)


%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@

