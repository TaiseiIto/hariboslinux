# OS specific symbols
ifeq ($(OS), Windows_NT)
BLANK =
DELIMITER = \$(BLANK)
SCRIPT_PREFIX = 
SCRIPT_SUFFIX = .bat
else
DELIMITER = /
SCRIPT_PREFIX = ./
SCRIPT_SUFFIX = .sh
endif

# floppy disk image file of the built operating system
IMAGE_FILE = haribos.img
BOOT_SECTORS = diskcontents/bootsector.bin
# AMLS
AML_SOURCES = $(wildcard diskcontents/*dsdt.txt)
AMLS = $(AML_SOURCES:.txt=.aml)
ASLS = $(AMLS:.aml=.dsl)
# Applications
APP_NAMES = $(shell for i in `ls -d src/apps/*/`; do basename $$i; done)
APPS = $(shell for i in $(APP_NAMES); do echo diskcontents/$${i}.com; done)
# files included in the floppy disk
FLOPPY_FILES = diskcontents/loaddisk.bin diskcontents/getmemmp.bin diskcontents/initscrn.bin diskcontents/mv2prtmd.bin diskcontents/dplydisk.bin diskcontents/lddskxtr.bin diskcontents/kernel.bin diskcontents/callbios.bin $(AMLS) $(APPS)

# tcp ports
DEBUG_PORT = 2159
VNC_PORT = 5900

# compiler
COMPILER = gcc
COMPILER_OUTPUT_OPTION = -o
COMPILER_WARNING_OPTION = -Wall -Wextra

# docker
DOCKER = docker
DOCKER_IMAGE = hariboslinux
DOCKER_IMAGE_TAG = latest
DOCKER_CONTAINER = hariboslinux
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

# convert from AML to ASL

build: $(IMAGE_FILE)

clean:
	rm -f $(AMLS) $(ASLS) $(IMAGE_PACKER) diskcontents/*.bin diskcontents/*.com *.bin *.o *.img
	make clean -C src

# Clean docker environment
clean-devenv:
	$(SCRIPT_PREFIX)script$(DELIMITER)clean-devenv$(SCRIPT_SUFFIX) $(DOCKER) $(DOCKER_IMAGE) $(DOCKER_CONTAINER)

# Debug the operating system onQEMU by gdb
debug: $(IMAGE_FILE) stop-qemu
	tmux new-session \; source-file tmux/.tmux.debug-haribos.conf

debug-qemu:
	$(EMULATOR) $(EMULATOR_BOOT_OPTION) $(EMULATOR_DRIVE_OPTION) $(EMULATOR_MEMORY_OPTION) $(EMULATOR_SERIAL_OPTION) $(EMULATOR_TIMEZONE_OPTION) $(EMULATOR_VIDEO_OPTION) $(EMULATOR_VNC_OPTION) $(EMULATOR_DEBUG_OPTION) | tee $(EMULATOR_SERIAL_OUT)

# Build docker environment
devenv:
	$(SCRIPT_PREFIX)script$(DELIMITER)devenv$(SCRIPT_SUFFIX) $(DOCKER) $(DOCKER_IMAGE) $(DOCKER_IMAGE_TAG) $(DOCKER_CONTAINER) $(VNC_PORT)

diskcontents/kernel.bin: src/kernel.bin
	cp $^ $@

diskcontents/%.bin: src/%.bin
	cp $^ $@

diskcontents/%.com: src/%.com
	cp $^ $@

diskcontents/%.dsl: diskcontents/%.aml
	iasl $^

diskcontents/%.aml: diskcontents/%.txt
	xxd -p -r $^ > $@

download-image:
	$(DOCKER) cp $(DOCKER_CONTAINER):/root/hariboslinux/$(IMAGE_FILE) .

# Only the developer can execute it.
# usage : $ make gitconfig KEY=<GitHub private key path> GPG=<.gnupg path>
gitconfig:
	$(DOCKER) cp $(KEY) $(DOCKER_CONTAINER):/root/hariboslinux/ssh/github && \
	$(DOCKER) cp $(GPG) $(DOCKER_CONTAINER):/root/.gnupg && \
	$(DOCKER) start $(DOCKER_CONTAINER) && \
	$(DOCKER) exec -it $(DOCKER_CONTAINER) /root/hariboslinux/git/gitconfig.sh && \
	$(DOCKER) stop $(DOCKER_CONTAINER)

# Rebuild the OS
rebuild: clean
	make 2>&1 | tee $(MAKE_OUT)
	! grep -i -e error -e warning $(MAKE_OUT)

# Rebuild docker environment
rebuild-devenv: clean-devenv
	make devenv

# run the OS on QEMU with interactive serial interface
run:
	make rebuild
	tmux new-session \; source-file tmux/.tmux.run-haribos.conf

# run the OS on QEMU
run-qemu: $(IMAGE_FILE) stop-qemu
	$(EMULATOR) $(EMULATOR_BOOT_OPTION) $(EMULATOR_DRIVE_OPTION) $(EMULATOR_MEMORY_OPTION) $(EMULATOR_SERIAL_OPTION) $(EMULATOR_TIMEZONE_OPTION) $(EMULATOR_VIDEO_OPTION) $(EMULATOR_VNC_OPTION) | tee $(EMULATOR_SERIAL_OUT)

src/kernel.bin: $(wildcard src/kernel/*.c src/kernel/*.h)
	make -C src

src/%.bin: src/%.s src/%.ld
	make -C src

src/%.com:
	make -C src

# stop QEMU
stop-qemu:
	for i in $$(ps ax | grep $(EMULATOR) | grep -v grep | awk '{print $$1}'); do kill $$i; done

# stop TMUX
stop: stop-qemu
	for i in $$(ps ax | grep tmux | grep -v grep | awk '{print $$1}'); do kill $$i; done

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

