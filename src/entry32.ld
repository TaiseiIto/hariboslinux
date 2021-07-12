OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = 0x00000000;

SECTIONS
{
	. = BASE;
	.text :
	{
		entry32.o(.text)
		entry32.o(.data)
	}
}

