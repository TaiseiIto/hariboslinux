OUTPUT_FORMAT("binary");

IPL_BASE = 0x00007c00;		/* memory address offset */

SECTIONS {
	. = IPL_BASE;
	. += 0x00000200;	/* end of the boot sector */
	end_of_the_boot_sector = .;
	. -= 0x00000002;
	.boot_sector_sign :
	{
		BYTE(0x55);
		BYTE(0xaa);
		. = end_of_the_boot_sector;
	}
	.first_FAT :
	{
		begin_of_the_first_FAT = .;
		BYTE(0xf0);
		BYTE(0xff);
		BYTE(0xff);
		. = begin_of_the_first_FAT + 0x00001200;
	}
	.second_FAT :
	{
		begin_of_the_second_FAT = .;
		BYTE(0xf0);
		BYTE(0xff);
		BYTE(0xff);
		. = begin_of_the_second_FAT + 0x00001200;
	}
	.file_informations :
	{
		. += 0x00001c00;
	}
	.file_contents :
	{
	}
	. = 0x00168000;
}

