COMPILER = gcc
LINKER = ld
EMULATOR = qemu-system-i386

all: haribos.img

clean:
	rm *.o *.img

haribos.o: haribos.s
	$(COMPILER) $^ -c -nostdlib -o $@

haribos.img: haribos.o
	$(LINKER) $^ -o $@ -T link.ld

run: haribos.img
	$(EMULATOR) -drive file=$^,format=raw,if=floppy -vga std

