OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		initscrn.o(.text)
		initscrn.o(.data)
	}
}

