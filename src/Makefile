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

APPNAMES = $(shell for i in `ls -d apps/*/`; do basename $$i; done)
APPS = $(APPNAMES:=.com)

BINSRCS = $(shell ls *.s)
BINARIES = $(BINSRCS:.s=.bin)

all: $(BINARIES) kernel.bin $(APPS)

clean:
	rm -f kernel.s *.bin *.o 
	make clean -C apps
	make clean -C libs
	make clean -C kernel

kernel.bin: kernel/kernel.bin
	cp $^ $@

kernel/kernel.bin: $(wildcard kernel/*.c kernel/*.h)
	make -C kernel

rebuild: clean
	make

%.bin: %.o
	$(LINKER) $^ $(LINKER_MAP_OPTION) $(@:.bin=.map) $(LINKER_OUTPUT_OPTION) $@ $(LINKER_SCRIPT_OPTION) $(@:.bin=.ld)

%.com:
	make -C libs
	make -C apps
	cp apps/$@ .

%.o: %.s
	$(COMPILER) $^ $(COMPILER_DONT_LINK_OPTION) $(COMPILER_DONT_USE_STDLIB_OPTION) $(COMPILER_OUTPUT_OPTION) $@ $(COMPILER_WARNING_OPTION)

# dont remove intermediate files
.PRECIOUS: %.bin %.o %.s

