ENTRY(main);
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE + SIZEOF_HEADERS;
	.text :
	{
		main.o(.text)
		main.o(.rodata)
		main.o(.data)
		main.o(.bss)
	}
}

