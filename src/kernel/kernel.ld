ENTRY(main);
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE;
	.text :
	{
		main.o(.text)
		font.o(.text)
		fontdata.o(.text)
		graphic.o(.text)
		io.o(.text)
		serial.o(.text)
		main.o(.rodata)
		font.o(.rodata)
		fontdata.o(.rodata)
		graphic.o(.rodata)
		io.o(.rodata)
		serial.o(.rodata)
		main.o(.data)
		font.o(.data)
		fontdata.o(.data)
		graphic.o(.data)
		io.o(.data)
		serial.o(.data)
		main.o(.bss)
		font.o(.bss)
		fontdata.o(.bss)
		graphic.o(.bss)
		io.o(.bss)
		serial.o(.bss)
	}
	/DISCARD/:{*(.eh_frame)}
}

