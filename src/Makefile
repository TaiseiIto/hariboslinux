# compiler
COMPILER = gcc
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_OUTPUT_OPTION = -o
COMPILER_WARNING_OPTION = -Wall -Wextra

# linker
LINKER = ld
LINKER_MAP_OPTION = -Map
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT_OPTION = -T

all: bootsector.bin loaddisk.bin getmemmp.bin initscrn.bin mv2prtmd.bin kernel.bin

clean:
	rm -f kernel.s *.bin *.o 
	make clean -C kernel

kernel.bin: kernel/kernel.bin
	cp $^ $@

kernel/kernel.bin: kernel/boot.h kernel/exception.c kernel/exception.h kernel/font/bitmap.txt kernel/font/translator.c kernel/font/Makefile kernel/font.c kernel/font.h kernel/gdt.c kernel/gdt.h kernel/graphic.c kernel/graphic.h kernel/idt.c kernel/idt.h kernel/io.h kernel/io.s kernel/kernel.ld kernel/keyboard.c kernel/keyboard.h kernel/main.c kernel/mouse.c kernel/mouse.h kernel/pic.c kernel/pic.h kernel/serial.c kernel/serial.h kernel/stack.c kernel/stack.h kernel/stdio.c kernel/stdio.h kernel/stdlib.h kernel/string.c kernel/string.h
	make -C kernel

rebuild: clean
	make

%.bin: %.o
	$(LINKER) $^ $(LINKER_MAP_OPTION) $(@:.bin=.map) $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

