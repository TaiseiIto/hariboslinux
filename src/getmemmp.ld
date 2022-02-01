OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		getmemmp.o(.text)
		getmemmp.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
	}
}

