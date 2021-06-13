OUTPUT_FORMAT("binary");

IPL_BASE = 0x00007c00;		/* memory address offset */

SECTIONS {
	. = IPL_BASE;
	.text :
	{
		haribos.o(.text)
	}
	. = IPL_BASE;
	. += 0x00000200;
	. -= 0x00000002;
	.boot_sector_sign :
	{
		BYTE(0x55);
		BYTE(0xaa);
	}
}

