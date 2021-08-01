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
		gdt.o(.text)
		graphic.o(.text)
		idt.o(.text)
		io.o(.text)
		pic.o(.text)
		serial.o(.text)
		stack.o(.text)
		stdio.o(.text)
		string.o(.text)
		main.o(.rodata)
		font.o(.rodata)
		fontdata.o(.rodata)
		gdt.o(.rodata)
		graphic.o(.rodata)
		idt.o(.rodata)
		io.o(.rodata)
		pic.o(.rodata)
		serial.o(.rodata)
		stack.o(.rodata)
		stdio.o(.rodata)
		string.o(.rodata)
		main.o(.data)
		font.o(.data)
		fontdata.o(.data)
		gdt.o(.data)
		graphic.o(.data)
		idt.o(.data)
		io.o(.data)
		pic.o(.data)
		serial.o(.data)
		stack.o(.data)
		stdio.o(.data)
		string.o(.data)
		main.o(.bss)
		font.o(.bss)
		fontdata.o(.bss)
		gdt.o(.bss)
		graphic.o(.bss)
		idt.o(.bss)
		io.o(.bss)
		pic.o(.bss)
		serial.o(.bss)
		stack.o(.bss)
		stdio.o(.bss)
		string.o(.bss)
	}
	/DISCARD/:{*(.eh_frame)}
	/DISCARD/:{*(.note.gnu.property)}
}

