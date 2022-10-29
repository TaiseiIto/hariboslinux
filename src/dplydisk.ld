OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST + LOADDISKADDR + LOADDISKSIZE + GETMEMMPSIZE + INITSCRNSIZE + MV2PRTMDSIZE;

SECTIONS
{
	. = BASE;
	.text :
	{
		dplydisk.o(.text)
	}
	.data :
	{
		dplydisk.o(.data)
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

