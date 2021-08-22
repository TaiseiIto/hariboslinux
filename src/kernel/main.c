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

BootInformation get_boot_information(void);

void main(void)
{
	BootInformation boot_information;
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
	boot_information = get_boot_information();
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	printf_screen(0x0000, 0 * CHAR_HEIGHT, foreground_color, background_color, "memory size = %#06X MiB", boot_information.memory_size);
	printf_screen(0x0000, 1 * CHAR_HEIGHT, foreground_color, background_color, "keyboard state = %#04X", boot_information.keyboard_state);
	printf_screen(0x0000, 2 * CHAR_HEIGHT, foreground_color, background_color, "last loaded cylinder = %#04X", boot_information.last_loaded_cylinder);
	printf_screen(0x0000, 3 * CHAR_HEIGHT, foreground_color, background_color, "last loaded head = %#04X", boot_information.last_loaded_head);
	printf_screen(0x0000, 4 * CHAR_HEIGHT, foreground_color, background_color, "last loaded sector = %#04X", boot_information.last_loaded_sector);
	for(unsigned short y = 0x0100; y < 0x0200; y++)for(unsigned short x = 0; x < 0x100; x++)
	{
		color.red = (unsigned char)x;
		color.green = (unsigned char)(y - 0x0100);
		color.blue = 0;
		put_dot(x, y, color);
	}
	color.red = 0xff;
	color.green = 0x00;
	color.blue = 0x00;
	fill_box(0x0100, 0x0100, 0x0100, 0x0100, color);
	color.red = 0x00;
	color.green = 0xff;
	color.blue = 0x00;
	fill_box(0x0200, 0x0100, 0x0100, 0x0100, color);
	color.red = 0x00;
	color.green = 0x00;
	color.blue = 0xff;
	fill_box(0x0300, 0x0100, 0x0100, 0x0100, color);
	new_line_serial_polling();
	while(1)hlt();
}

BootInformation get_boot_information(void)
{
	BootInformation boot_information;
	reads(boot_information_segment_selector, (void *)0x00000000, (void *)&boot_information, sizeof(boot_information));
	return boot_information;
}

