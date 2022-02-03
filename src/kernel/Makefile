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

OBJECTS = cmos.o disk.o event.o exception.o font.o fontdata.o gdt.o graphic.o idt.o interrupt_handlers.o keyboard.o libgcc.o main.o math.o memory.o mouse.o mouse_cursor.o parallel.o peripheral.o pic.o pit.o queue.o rtc.o serial.o sheet.o stack.o stdio.o string.o task.o timer.o window.o

all: kernel.bin

clean:
	rm -f $$(ls *.bin *.o *.s | grep -v "^interrupt_gates.s$$\|^io.s$$")
	make clean -C font

kernel.bin: $(OBJECTS) interrupt_gates.o io.o
	$(LINKER) $^ $(LINKER_I386_OPTION) $(LINKER_MAP_OPTION) $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

fontdata.c: font/fontdata.c
	cp $^ $@

font/fontdata.c: font/bitmap.txt font/Makefile font/translator.c
	make -C font

interrupt_gates.o: interrupt_gates.s

io.o: io.s

rebuild: clean
	make

$(OBJECTS): $(@:.o=.s)

$(OBJECTS:.o=.s): $(@:.s=.c)

%.o: %.s
	$(COMPILER) $^ $(COMPILER_CODE32_OPTION) $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

%.s: %.c
	$(COMPILER) $< $(COMPILER_CODE32_OPTION) $(COMPILER_OUTPUT_ASSEMBLY_OPTION) $(COMPILER_FNO_BUILTIN_OPTION) $(COMPILER_FNO_PIE_OPTION) $(COMPILER_OUTPUT_OPTION) $@

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

