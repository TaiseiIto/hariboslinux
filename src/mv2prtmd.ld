OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE + INITSCRNSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		mv2prtmd.o(.text)
		mv2prtmd.o(.data)
	}
}

