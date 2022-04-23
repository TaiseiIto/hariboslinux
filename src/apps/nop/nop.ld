ENTRY(_start)
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE;
	.header :
	{
		LONG(0x00100000); /* Size of heap and stack */
	}
	.text :
	{
		crt.o(.text)
		nop.o(.text)
	}
	.rodata :
	{
		crt.o(.rodata)
		nop.o(.rodata)
	}
	.data :
	{
		crt.o(.data)
		nop.o(.data)
	}
	.bss :
	{
		crt.o(.bss)
		nop.o(.bss)
	}
	COMMON :
	{
		crt.o(COMMON)
		nop.o(COMMON)
	}
	.common_deletion_prevention :
	{
		BYTE(0xff);
	}
	/DISCARD/ :
	{
		*(.eh_frame)
		*(.note.gnu.property)
	}
}

