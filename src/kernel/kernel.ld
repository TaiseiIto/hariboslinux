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
		global_descriptor_table.o(.text)
		graphic.o(.text)
		io.o(.text)
		serial.o(.text)
		stack.o(.text)
		stdio.o(.text)
		string.o(.text)
		main.o(.rodata)
		font.o(.rodata)
		fontdata.o(.rodata)
		global_descriptor_table.o(.rodata)
		graphic.o(.rodata)
		io.o(.rodata)
		serial.o(.rodata)
		stack.o(.rodata)
		stdio.o(.rodata)
		string.o(.rodata)
		main.o(.data)
		font.o(.data)
		fontdata.o(.data)
		global_descriptor_table.o(.data)
		graphic.o(.data)
		io.o(.data)
		serial.o(.data)
		stack.o(.data)
		stdio.o(.data)
		string.o(.data)
		main.o(.bss)
		font.o(.bss)
		fontdata.o(.bss)
		global_descriptor_table.o(.bss)
		graphic.o(.bss)
		io.o(.bss)
		serial.o(.bss)
		stack.o(.bss)
		stdio.o(.bss)
		string.o(.bss)
	}
	/DISCARD/:{*(.eh_frame)}
}

