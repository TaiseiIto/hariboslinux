ENTRY(main);
OUTPUT_FORMAT("binary");

BASE = 0x00000000;

SECTIONS
{
	. = BASE + SIZEOF_HEADERS;
	.text :
	{
		main.o(.text)
		io.o(.text)
		serial.o(.text)
		main.o(.rodata)
		io.o(.rodata)
		serial.o(.rodata)
		main.o(.data)
		io.o(.data)
		serial.o(.data)
		main.o(.bss)
		io.o(.bss)
		serial.o(.bss)
	}
}

