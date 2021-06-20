# floppy disk image file of the built operating system
IMAGE_FILE = haribos.img
BOOT_SECTORS = boot_sector.bin
# files included in the floppy disk
FLOPPY_FILES = test.txt

# compiler
COMPILER = gcc
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_OUTPUT_OPTION = -o
COMPILER_WARNING_OPTION = -Wall -Wextra

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
EMULATOR_BOOT_OPTION = -boot order=a
# a raw image of the floppy disk
EMULATOR_DRIVE_OPTION = -drive file=$(IMAGE_FILE),format=raw,if=floppy
# VESA VBE 2.0
EMULATOR_VIDEO_OPTION = -vga std
# virtual network computing for all ip address
EMULATOR_VNC_OPTION = -vnc :0

# linker
LINKER = ld
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT_OPTION = -T

# image paccker
IMAGE_PACKER = ./pack

# build the operating system
all: build

%.bin: %.o
	$(LINKER) $^ $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

build: $(IMAGE_FILE)

clean:
	rm -f pack *.bin *.o *.img

docker-build:
	$(DOCKER) build --no-cache -t $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_TAG) .

docker-login:
	$(DOCKER) attach $(DOCKER_CONTAINER_NAME)

docker-remove-container:
	$(DOCKER) rm $(DOCKER_CONTAINER_NAME)

docker-remove-image:
	$(DOCKER) rmi $(DOCKER_IMAGE_NAME)

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
	$(EMULATOR) $(EMULATOR_BOOT_OPTION) $(EMULATOR_DRIVE_OPTION) $(EMULATOR_VIDEO_OPTION) $(EMULATOR_VNC_OPTION)

update: update-repository
	make build

update-repository:
	git pull origin main

$(IMAGE_FILE): $(IMAGE_PACKER) $(BOOT_SECTORS) $(FLOPPY_FILES)
	$(IMAGE_PACKER) $@ $(BOOT_SECTORS) $(FLOPPY_FILES)

$(IMAGE_PACKER): $(addsuffix .c, $(IMAGE_PACKER))
	$(COMPILER) $^ $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

