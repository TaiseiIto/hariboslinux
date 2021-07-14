OUTPUT_FORMAT("binary");

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

