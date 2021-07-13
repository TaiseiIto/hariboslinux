OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = 0x00000000;

SECTIONS
{
	. = BASE;
	.text :
	{
		kernel.o(.text)
		kernel.o(.data)
	}
}

