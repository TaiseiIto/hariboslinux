all: haribos.img

haribos.img: haribos.s link.ls
	gcc $< -nostdlib -o $@ -T link.ls
