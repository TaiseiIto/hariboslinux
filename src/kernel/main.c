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
	Color background_color;
	Color color;
	Color foreground_color;
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
		color.red = (unsigned char)x;
		color.green = (unsigned char)y;
		color.blue = 0;
		put_dot(x, y, color);
	}
	color.red = 0xff;
	color.green = 0x00;
	color.blue = 0x00;
	fill_box(0x0100, 0x0000, 0x0100, 0x0100, color);
	color.red = 0x00;
	color.green = 0xff;
	color.blue = 0x00;
	fill_box(0x0200, 0x0000, 0x0100, 0x0100, color);
	color.red = 0x00;
	color.green = 0x00;
	color.blue = 0xff;
	fill_box(0x0300, 0x0000, 0x0100, 0x0100, color);
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	printf_screen(0, 0x0100, foreground_color, background_color, "Hello, World!\nHello, hariboslinux!\n");
	while(1)hlt();
}

