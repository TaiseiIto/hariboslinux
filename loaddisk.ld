OUTPUT_FORMAT("binary");

BASE = 0x00007c00 + 0x00004200;

SECTIONS
{
	. = BASE;
	.text :
	{
		loaddisk.o(.text)
		loaddisk.o(.data)
	}
}

