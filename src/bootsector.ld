OUTPUT_FORMAT("binary");

INCLUDE global.ld

BASE = LOADDEST;		/* memory address offset */

SECTIONS
{
	. = BASE;
	.text :
	{
		bootsector.o(.text)
		bootsector.o(.data)
	}
	. = BASE;
	. += 0x00000200;
	. -= 0x00000002;
	.boot_sector_sign :
	{
		BYTE(0x55);
		BYTE(0xaa);
	}
	/DISCARD/ :
	{
		*(.eh_frame)
	}
}

