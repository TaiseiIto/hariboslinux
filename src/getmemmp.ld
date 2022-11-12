OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		getmemmp.o(.text)
	}
	.data :
	{
		getmemmp.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

