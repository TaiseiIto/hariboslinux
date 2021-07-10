OUTPUT_FORMAT("binary");

BASE = 0x00007c00 + 0x00004200 + 0x00000600;

SECTIONS
{
	. = BASE;
	.text :
	{
		initscrn.o(.text)
		initscrn.o(.data)
	}
}

