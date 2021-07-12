OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE + INITSCRNSIZE + MV2PRTMDSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		entry32.o(.text)
		entry32.o(.data)
	}
}

