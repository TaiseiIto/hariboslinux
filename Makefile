ASSEMBLY_FILES = haribos.s

# compiler
COMPILER = gcc
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_OUTPUT_OPTION = -o

# docker
DOCKER_IMAGE_NAME = hariboslinux
DOCKER_IMAGE_TAG = latest
DOCKER_CONTAINER_NAME = hariboslinux
DOCKER_CONTAINER_SHELL = /bin/sh

# emulator
# emulate intel 386 processor
EMULATOR = qemu-system-i386
# a raw image of the floppy disk
EMULATOR_DRIVE_OPTION = -drive file=$(IMAGE_FILE),format=raw,if=floppy
# VESA VBE 2.0
EMULATOR_VIDEO_OPTION = -vga std

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
	docker build -t $(DOCKER_IMAGE_NAME):$(DOCKER_IMAGE_TAG) .

docker-login:
	docker exec -i -t $(DOCKER_CONTAINER_NAME) $(DOCKER_CONTAINER_SHELL)

docker-run:
	docker run --name $(DOCKER_CONTAINER_NAME) -i -t $(DOCKER_IMAGE_NAME) $(DOCKER_CONTAINER_SHELL)

docker-start:
	docker start $(DOCKER_CONTAINER_NAME)

docker-stop:
	docker stop $(DOCKER_CONTAINER_NAME)

rebuild: clean
	make build

# run the operating system on QEMU
run: $(IMAGE_FILE)
	$(EMULATOR) $(EMULATOR_DRIVE_OPTION) $(EMULATOR VIDEO OPTION)

$(IMAGE_FILE): $(ASSEMBLY_FILES:.s=.o)
	$(LINKER) $^ $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(LINKER_SCRIPT)


%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@

