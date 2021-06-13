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
	first_FAT_start = .;
	.first_FAT :
	{
		BYTE(0xf0);
		BYTE(0xff);
		BYTE(0xff);
	}
	. = first_FAT_start;
	. += 0x00001200;
	second_FAT_start = .;
	.second_FAT :
	{
		BYTE(0xf0);
		BYTE(0xff);
		BYTE(0xff);
	}
	. = second_FAT_start;
	. += 0x00001200;
	file_informations_start = .;
	.file_informations :
	{
		BYTE(0x00);
	}
	. = file_informations_start;
	. += 0x1c00;
	file_contents_start = .;
	.file_contents :
	{
		BYTE(0x00);
	}
	. = file_contents_start;
	. = IPL_BASE;
	. += 0x00168000;
	.enf_of_floppy_disk :
	{
	}
}

