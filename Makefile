ASSEMBLY_FILES = haribos.s
COMPILER = gcc
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_OUTPUT_OPTION = -o
EMULATOR = qemu-system-i386
EMULATOR_DRIVE_OPTION = -drive file=$(IMAGE_FILE),format=raw,if=floppy
EMULATOR_VIDEO_OPTION = -vga std
IMAGE_FILE = haribos.img
LINKER = ld
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT = link.ld
LINKER_SCRIPT_OPTION = -T

all: $(IMAGE_FILE)

clean:
	rm *.o *.img

$(IMAGE_FILE): $(ASSEMBLY_FILES:.s=.o)
	$(LINKER) $^ $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(LINKER_SCRIPT)

run: $(IMAGE_FILE)
	$(EMULATOR) $(EMULATOR_DRIVE_OPTION) $(EMULATOR VIDEO OPTION)

%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@

