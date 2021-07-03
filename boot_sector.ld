OUTPUT_FORMAT("binary");

BASE = 0x00007c00;		/* memory address offset */

SECTIONS
{
	. = BASE;
	.text :
	{
		boot_sector.o(.text)
		boot_sector.o(.data)
	}
	. = BASE;
	. += 0x00000200;
	. -= 0x00000002;
	.boot_sector_sign :
	{
		BYTE(0x55);
		BYTE(0xaa);
	}
}

