all: haribos.img

run: haribos.img
	qemu-system-i386 -drive file=$^,format=raw,if=floppy -vga std

haribos.o: haribos.s
	gcc $^ -c -nostdlib -o $@

haribos.img: haribos.o
	ld $^ -o $@ -T link.ld
