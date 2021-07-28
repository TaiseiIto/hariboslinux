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
LINKER_OUTPUT_OPTION = -o
LINKER_SCRIPT_OPTION = -T

all: kernel.bin

clean:
	rm -f *.bin *.o main.s font.s fontdata.s graphic.s main.s stack.s serial.s stdio.s string.s
	make clean -C font

kernel.bin: font.o fontdata.o gdt.o graphic.o main.o io.o serial.o stack.o stdio.o string.o
	$(LINKER) $^ $(LINKER_I386_OPTION) $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

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

io.o: io.s

main.o: main.s

main.s: main.c io.h serial.h

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

