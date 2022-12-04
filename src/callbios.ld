OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST;

SECTIONS
{
	. = BASE;
	.text :
	{
		callbios.o(.text)
	}
	.data :
	{
		callbios.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

