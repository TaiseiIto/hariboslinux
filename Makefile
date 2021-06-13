all: haribos.img

run: haribos.img
	qemu-system-i386 -drive file=$^,format=raw,if=floppy -vga std

haribos.img: haribos.s link.ls
	gcc $< -nostdlib -o $@ -T link.ls
