OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR;

SECTIONS
{
	. = BASE;
	.text :
	{
		loaddisk.o(.text)
		loaddisk.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
	}
}

