#include "boot.h"
#include "font.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "serial.h"
#include "stdio.h"

// get BootInformation structure at 0x00007bf8
BootInformation get_boot_information(void);

void main(void)
{
	BootInformation boot_information;
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n\n");
	init_gdt();
	boot_information = get_boot_information();
	printf_serial_polling("memory size = %#06X MiB\n", boot_information.memory_size);
	printf_serial_polling("screen width = %#06X pixels\n", boot_information.screen_width);
	printf_serial_polling("screen height = %#06X pixels\n", boot_information.screen_height);
	printf_serial_polling("bits per pixel = %#04X bits\n", boot_information.bits_per_pixel);
	printf_serial_polling("keyboard state = %#04X\n", boot_information.keyboard_state);
	printf_serial_polling("last loaded sylinder = %#04X\n", boot_information.last_loaded_cylinder);
	printf_serial_polling("last loaded head = %#04X\n", boot_information.last_loaded_head);
	printf_serial_polling("last loaded sector = %#04X\n", boot_information.last_loaded_sector);
	init_screen(boot_information.screen_width, boot_information.screen_height);
	printf_screen(0, 0, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "memory size = %#06X MiB", boot_information.memory_size);
	printf_screen(0, CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "screen size = (%#06X, %#06X) pixels", boot_information.screen_width, boot_information.screen_height);
	printf_screen(0, 2 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "bits per pixel = %#04X bits", boot_information.bits_per_pixel);
	printf_screen(0, 3 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "keyboard state = %#04X", boot_information.keyboard_state);
	printf_screen(0, 4 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "last loaded cylinder = %#04X", boot_information.last_loaded_cylinder);
	printf_screen(0, 5 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "last loaded head = %#04X", boot_information.last_loaded_head);
	printf_screen(0, 6 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00), "last loaded sector = %#04X", boot_information.last_loaded_sector);
	while(1)hlt();
}

// get BootInformation structure
BootInformation get_boot_information(void)
{
	BootInformation boot_information;
	reads(boot_information_segment_selector, (void *)0x00000000, (void *)&boot_information, sizeof(boot_information));
	return boot_information;
}

