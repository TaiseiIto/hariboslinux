# compiler
COMPILER = gcc
COMPILER_CODE32_OPTION = -m32
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
COMPILER_FNO_BUILTIN_OPTION = -fno-builtin
COMPILER_FNO_PIE_OPTION = -fno-pie
COMPILER_OUTPUT_ASSEMBLY_OPTION = -S
COMPILER_OUTPUT_OPTION = -o
COMPILER_WARNING_OPTION = -Wall -Wextra

# linker
LINKER = ld
LINKER_I386_OPTION = -m elf_i386
LINKER_MAP_OPTION = -Map kernel.map
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT_OPTION = -T

all: kernel.bin

clean:
	rm -f $$(ls *.bin *.o *.s | grep -v ^io.s$$)
	make clean -C font

kernel.bin: exception.o font.o fontdata.o gdt.o graphic.o idt.o io.o keyboard.o libgcc.o main.o memory.o mouse.o pic.o serial.o stack.o stdio.o string.o
	$(LINKER) $^ $(LINKER_I386_OPTION) $(LINKER_MAP_OPTION) $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

exception.o: exception.s

exception.s: exception.c

font.o: font.s

font.s: font.c

fontdata.o: fontdata.s

fontdata.s: fontdata.c

fontdata.c: font/fontdata.c
	cp $^ $@

font/fontdata.c: font/bitmap.txt font/Makefile font/translator.c
	make -C font

gdt.o: gdt.s

gdt.s: gdt.c

graphic.o: graphic.s

graphic.s: graphic.c

idt.o: idt.s

idt.s: idt.c

io.o: io.s

keyboard.o: keyboard.s

keyboard.s: keyboard.c

libgcc.o: libgcc.s

libgcc.s: libgcc.c

main.o: main.s

main.s: main.c

memory.o: memory.s

memory.s: memory.c

mouse.o: mouse.s

mouse.s: mouse.c

pic.o: pic.s

pic.s: pic.c

serial.o: serial.s

serial.s: serial.c serial.h

stack.o: stack.s

stack.s: stack.c

stdio.o: stdio.s

stdio.s: stdio.c

string.o: string.s

string.s: string.c

rebuild: clean
	make

%.o: %.s
	$(COMPILER) $^ $(COMPILER_CODE32_OPTION) $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

%.s: %.c
	$(COMPILER) $< $(COMPILER_CODE32_OPTION) $(COMPILER_OUTPUT_ASSEMBLY_OPTION) $(COMPILER_FNO_BUILTIN_OPTION) $(COMPILER_FNO_PIE_OPTION) $(COMPILER_OUTPUT_OPTION) $@

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

