#include "font.h"
#include "graphic.h"
#include "io.h"
#include "segment.h"
#include "serial.h"
#include "stdio.h"

typedef struct
{
	unsigned short memory_size;	// MiB
	unsigned short screen_width;
	unsigned short screen_height;
	unsigned char bits_per_pixel;
	unsigned char keyboard_state;
} BootInformation;

// get BootInformation structure at 0x00007bf8
BootInformation get_boot_information(void);
void __stack_chk_fail(void);

void main(void)
{
	BootInformation boot_information;
	char string[0x100];
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n\n");
	boot_information = get_boot_information();
	printf_serial_polling("memory size = %#06X Mib\n", boot_information.memory_size);
	printf_serial_polling("screen width = %#06X pixels\n", boot_information.screen_width);
	printf_serial_polling("screen height = %#06X pixels\n", boot_information.screen_height);
	printf_serial_polling("bits per pixel = %#04X bits\n", boot_information.bits_per_pixel);
	printf_serial_polling("keyboard state = %#04X\n", boot_information.keyboard_state);
	init_screen(boot_information.screen_width, boot_information.screen_height);
	sprintf(string, "memory size = %#06X Mib", boot_information.memory_size);
	put_string(string, 0, 0, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
	sprintf(string, "screen width = %#06X pixels", boot_information.screen_width);
	put_string(string, 0, CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
	sprintf(string, "screen height = %#06X pixels", boot_information.screen_height);
	put_string(string, 0, 2 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
	sprintf(string, "bits per pixel = %#04X bits", boot_information.bits_per_pixel);
	put_string(string, 0, 3 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
	sprintf(string, "keyboard state = %#04X", boot_information.keyboard_state);
	put_string(string, 0, 4 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
	while(1)hlt();
}

// get BootInformation structure at 0x00007bf8
BootInformation get_boot_information(void)
{
	BootInformation boot_information;
	reads(WHOLE_SEGMENT, (void *)0x00007bf8, (void *)&boot_information, sizeof(boot_information));
	return boot_information;
}

void __stack_chk_fail(void)
{
	print_serial_polling("__stack_chk_fail called!\n");
}

