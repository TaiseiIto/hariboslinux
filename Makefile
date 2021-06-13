ASSEMBLY_FILES = haribos.s
COMPILER = gcc
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_OUTPUT_OPTION = -o
EMULATOR = qemu-system-i386
LINKER = ld
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT = link.ld
LINKER_SCRIPT_OPTION = -T

all: haribos.img

clean:
	rm *.o *.img

haribos.img: $(ASSEMBLY_FILES:.s=.o)
	$(LINKER) $^ $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(LINKER_SCRIPT)

run: haribos.img
	$(EMULATOR) -drive file=$^,format=raw,if=floppy -vga std

%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@

