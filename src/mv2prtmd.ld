OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE + GETMEMMPSIZE + INITSCRNSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		mv2prtmd.o(.text)
		mv2prtmd.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

