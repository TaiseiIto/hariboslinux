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
		hello.o(.text)
		io.o(.text)
		system_call.o(.text)
	}
	rodata_base = .;
	.rodata :
	{
		crt.o(.rodata)
		hello.o(.rodata)
		io.o(.rodata)
		system_call.o(.rodata)
	}
	data_base = .;
	.data :
	{
		crt.o(.data)
		hello.o(.data)
		io.o(.data)
		system_call.o(.data)
	}
	bss_base = .;
	.bss :
	{
		crt.o(.bss)
		hello.o(.bss)
		io.o(.bss)
		system_call.o(.bss)
	}
	common_base = .;
	COMMON :
	{
		crt.o(COMMON)
		hello.o(COMMON)
		io.o(COMMON)
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

