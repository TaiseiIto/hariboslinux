# floppy disk image file of the built operating system
IMAGE_FILE = haribos.img
BOOT_SECTORS = diskcontents/bootsector.bin
# Applications
APP_NAMES = $(shell for i in `ls -d src/apps/*/`; do basename $$i; done)
APPS = $(shell for i in $(APP_NAMES); do echo diskcontents/$${i}.com; done)
# files included in the floppy disk
FLOPPY_FILES = diskcontents/loaddisk.bin diskcontents/getmemmp.bin diskcontents/initscrn.bin diskcontents/mv2prtmd.bin diskcontents/dplydisk.bin diskcontents/kernel.bin $(APPS) diskcontents/test0.txt diskcontents/test1.txt diskcontents/test2.txt diskcontents/test3.txt diskcontents/test4.txt

# tcp ports
DEBUG_PORT = 2159
VNC_PORT = 5900

# compiler
COMPILER = gcc
COMPILER_OUTPUT_OPTION = -o
COMPILER_WARNING_OPTION = -Wall -Wextra

# docker
DOCKER = docker
DOCKER_IMAGE_NAME = hariboslinux
DOCKER_IMAGE_TAG = latest
DOCKER_CONTAINER_NAME = hariboslinux
DOCKER_CONTAINER_SHELL = /bin/sh
DOCKER_VNC_PORT_OPTION = -p $(VNC_PORT):$(VNC_PORT)

# emulator
# emulate intel 386 processor
EMULATOR = qemu-system-i386
EMULATOR_BOOT_OPTION = -boot order=a
# debug by gdb through tcp:2159
EMULATOR_DEBUG_OPTION = -S -gdb tcp::$(DEBUG_PORT)
# a raw image of the floppy disk
EMULATOR_DRIVE_OPTION = -drive file=$(IMAGE_FILE),format=raw,if=floppy
# memory size assigned for the emulator 
EMULATOR_MEMORY_OPTION = -m 4G
# serial console
EMULATOR_SERIAL_OPTION = -serial stdio
EMULATOR_SERIAL_OUT = serialout.txt
# timezone
EMULATOR_TIMEZONE_OPTION = -rtc base=localtime
# VESA VBE 2.0
EMULATOR_VIDEO_OPTION = -vga std
# virtual network computing for all ip address
EMULATOR_VNC_OPTION = -vnc :0

# image paccker
IMAGE_PACKER = imagepacker/imagepacker

MAKE_OUT = makeout.txt

# build the operating system
all: build

build: $(IMAGE_FILE)

clean:
	rm -f diskcontents/*.bin diskcontents/*.com $(IMAGE_PACKER) *.bin *.o *.img
	make clean -C src

diskcontents/kernel.bin: src/kernel.bin
	cp $^ $@

diskcontents/%.bin: src/%.bin
	cp $^ $@

diskcontents/%.com: src/%.com
	cp $^ $@

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

download-image:
	$(DOCKER) cp $(DOCKER_CONTAINER_NAME):/root/hariboslinux/$(IMAGE_FILE) .

# debug the operating system onQEMU by gdb
debug: $(IMAGE_FILE) stop
	($(EMULATOR) $(EMULATOR_BOOT_OPTION) $(EMULATOR_DRIVE_OPTION) $(EMULATOR_MEMORY_OPTION) $(EMULATOR_SERIAL_OPTION) $(EMULATOR_VIDEO_OPTION) $(EMULATOR_VNC_OPTION) $(EMULATOR_DEBUG_OPTION) &) && \
	gdb < debuggee_input.txt

# Only the developer can execute it.
# usage : $ make gitconfig KEY=<GitHub private key path> GPG=<.gnupg path>
gitconfig:
	$(DOCKER) cp $(KEY) $(DOCKER_CONTAINER_NAME):/root/hariboslinux/ssh/github && \
	$(DOCKER) cp $(GPG) $(DOCKER_CONTAINER_NAME):/root/.gnupg && \
	make docker-start && \
	$(DOCKER) exec -it $(DOCKER_CONTAINER_NAME) /root/hariboslinux/git/gitconfig.sh

# rebuild the OS
rebuild: clean
	make 2>&1 | tee $(MAKE_OUT)
	! grep -i -e error -e warning $(MAKE_OUT)

# run the OS on QEMU with interactive serial interface
run:
	make rebuild
	tmux new-session \; source-file tmux/.tmux.run-haribos.conf

# run the OS on QEMU
run-qemu: $(IMAGE_FILE) stop
	$(EMULATOR) $(EMULATOR_BOOT_OPTION) $(EMULATOR_DRIVE_OPTION) $(EMULATOR_MEMORY_OPTION) $(EMULATOR_SERIAL_OPTION) $(EMULATOR_TIMEZONE_OPTION) $(EMULATOR_VIDEO_OPTION) $(EMULATOR_VNC_OPTION) | tee $(EMULATOR_SERIAL_OUT)

src/kernel.bin: $(wildcard src/kernel/*.c src/kernel/*.h)
	make -C src

src/%.bin: src/%.s src/%.ld
	make -C src

src/%.com:
	make -C src

# stop QEMU
stop:
	for i in $$(ps ax | grep $(EMULATOR) | grep -v grep | awk '{print $$1}'); do kill $$i; done

# update the OS
update: update-repository
	make

update-repository:
	git pull origin main

$(IMAGE_FILE): $(IMAGE_PACKER) $(BOOT_SECTORS) $(FLOPPY_FILES)
	$(IMAGE_PACKER) $@ $(BOOT_SECTORS) $(FLOPPY_FILES)

$(IMAGE_PACKER): $(addsuffix .c, $(IMAGE_PACKER))
	$(COMPILER) $^ $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

