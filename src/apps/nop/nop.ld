ENTRY(_start)
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE;
	.text :
	{
		crt.o(.text)
		io.o(.text)
		nop.o(.text)
	}
	.rodata :
	{
		crt.o(.rodata)
		io.o(.rodata)
		nop.o(.rodata)
	}
	.data :
	{
		crt.o(.data)
		io.o(.data)
		nop.o(.data)
	}
	.bss :
	{
		crt.o(.bss)
		io.o(.bss)
		nop.o(.bss)
	}
	COMMON :
	{
		crt.o(COMMON)
		io.o(COMMON)
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

