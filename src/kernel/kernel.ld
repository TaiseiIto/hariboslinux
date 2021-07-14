ENTRY(main);
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE + SIZEOF_HEADERS;
	.text :
	{
		kernel.o(.text)
		kernel.o(.rodata)
		kernel.o(.data)
		kernel.o(.bss)
	}
}

