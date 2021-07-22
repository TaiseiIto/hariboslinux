# compiler
COMPILER = gcc
COMPILER_CODE32_OPTION = -m32
COMPILER_DONT_LINK_OPTION = -c
COMPILER_DONT_USE_STDLIB_OPTION = -nostdlib
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
	rm -f *.bin *.o main.s

kernel.bin: main.o io.o serial.o
	$(LINKER) $^ $(LINKER_I386_OPTION) $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

io.o: io.s

main.o: main.s

main.s: main.c io.h serial.h

serial.o: serial.s

serial.s: serial.c serial.h

rebuild: clean
	make

%.o: %.s
	$(COMPILER) $^ $(COMPILER_CODE32_OPTION) $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

%.s:
	$(COMPILER) $< $(COMPILER_CODE32_OPTION) $(COMPILER_OUTPUT_ASSEMBLY_OPTION) $(COMPILER_FNO_PIE_OPTION) $(COMPILER_OUTPUT_OPTION) $@

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

