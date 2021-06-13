ASSEMBLY_FILES = haribos.s
COMPILER = gcc
EMULATOR = qemu-system-i386
LINKER = ld

all: haribos.img

clean:
	rm *.o *.img

haribos.img: $(ASSEMBLY_FILES:.s=.o)
	$(LINKER) $^ -o $@ -T link.ld

run: haribos.img
	$(EMULATOR) -drive file=$^,format=raw,if=floppy -vga std

%.o: %.s
	$(COMPILER) $^ -c -nostdlib -o $@

