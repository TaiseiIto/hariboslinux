OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR;

SECTIONS
{
	. = BASE;
	.text :
	{
		loaddisk.o(.text)
	}
	.data :
	{
		loaddisk.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

