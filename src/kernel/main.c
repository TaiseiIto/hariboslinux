#include "boot.h"
#include "font.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "serial.h"
#include "stdio.h"

void main(void)
{
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n\n");
	init_gdt();
	init_idt();
	init_pic();
	sti();
	init_keyboard();
	init_mouse();
	init_screen();
	new_line_serial_polling();
	for(unsigned short y = 0; y < 0x100; y++)for(unsigned short x = 0; x < 0x100; x++)
	{
		Color color;
		color.red = (unsigned char)x;
		color.green = (unsigned char)y;
		color.blue = 0;
		put_dot(x, y, color);
	}
	while(1)hlt();
}

