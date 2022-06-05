ENTRY(_start)
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE;
	.header :
	{
		LONG(text_base);
		LONG(rodata_base);
		LONG(data_base);
		LONG(bss_base);
		LONG(common_base);
		LONG(common_deletion_prevention_base);
		LONG(heap_and_stack_base);
		_heap_and_stack_size = 0x00100000;
		LONG(_heap_and_stack_size);
	}
	text_base = .;
	.text :
	{
		crt.o(.text)
		chain_string.o(.text)
		ctype.o(.text)
		dev.o(.text)
		free.o(.text)
		io.o(.text)
		libgcc.o(.text)
		math.o(.text)
		stack.o(.text)
		stdio.o(.text)
		stdlib.o(.text)
		string.o(.text)
		system_call.o(.text)
	}
	rodata_base = .;
	.rodata :
	{
		crt.o(.rodata)
		chain_string.o(.rodata)
		ctype.o(.rodata)
		dev.o(.rodata)
		free.o(.rodata)
		io.o(.rodata)
		libgcc.o(.rodata)
		math.o(.rodata)
		stack.o(.rodata)
		stdio.o(.rodata)
		stdlib.o(.rodata)
		string.o(.rodata)
		system_call.o(.rodata)
	}
	data_base = .;
	.data :
	{
		crt.o(.data)
		chain_string.o(.data)
		ctype.o(.data)
		dev.o(.data)
		free.o(.data)
		io.o(.data)
		libgcc.o(.data)
		math.o(.data)
		stack.o(.data)
		stdio.o(.data)
		stdlib.o(.data)
		string.o(.data)
		system_call.o(.data)
	}
	bss_base = .;
	.bss :
	{
		crt.o(.bss)
		chain_string.o(.bss)
		ctype.o(.bss)
		dev.o(.bss)
		free.o(.bss)
		io.o(.bss)
		libgcc.o(.bss)
		math.o(.bss)
		stack.o(.bss)
		stdio.o(.bss)
		stdlib.o(.bss)
		string.o(.bss)
		system_call.o(.bss)
	}
	common_base = .;
	COMMON :
	{
		crt.o(COMMON)
		chain_string.o(COMMON)
		ctype.o(COMMON)
		dev.o(COMMON)
		free.o(COMMON)
		io.o(COMMON)
		libgcc.o(COMMON)
		math.o(COMMON)
		stack.o(COMMON)
		stdio.o(COMMON)
		stdlib.o(COMMON)
		string.o(COMMON)
		system_call.o(COMMON)
	}
	common_deletion_prevention_base = .;
	.common_deletion_prevention :
	{
		BYTE(0xff);
	}
	heap_and_stack_base = .;
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

