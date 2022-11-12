OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE + GETMEMMPSIZE + INITSCRNSIZE + MV2PRTMDSIZE + DPLYDISKSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		lddskxtr.o(.text)
	}
	.data :
	{
		lddskxtr.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

