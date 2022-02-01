OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE + GETMEMMPSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		initscrn.o(.text)
		initscrn.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

