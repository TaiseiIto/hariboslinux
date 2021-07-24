#include "font.h"
#include "graphic.h"
#include "io.h"
#include "segment.h"
#include "serial.h"


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
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	new_line_serial_polling();
	boot_information = get_boot_information();
	print_serial_polling("memory size = 0x");
	print_word_hex_serial_polling(boot_information.memory_size);
	new_line_serial_polling();
	print_serial_polling("screen width = 0x");
	print_word_hex_serial_polling(boot_information.screen_width);
	new_line_serial_polling();
	print_serial_polling("screen height = 0x");
	print_word_hex_serial_polling(boot_information.screen_height);
	new_line_serial_polling();
	print_serial_polling("bits per pixel = 0x");
	print_byte_hex_serial_polling(boot_information.bits_per_pixel);
	new_line_serial_polling();
	print_serial_polling("keyboard state = 0x");
	print_byte_hex_serial_polling(boot_information.keyboard_state);
	new_line_serial_polling();
	init_screen(boot_information.screen_width, boot_information.screen_height);
	put_string("Hello, World!", 0, 0, get_color(0x00, 0x00, 0x05), get_color(0x00, 0x00, 0x00));
	put_string("Hello, World!", 0, CHAR_HEIGHT, get_color(0x00, 0x05, 0x00), get_color(0x00, 0x00, 0x00));
	put_string("Hello, World!", 0, 2 * CHAR_HEIGHT, get_color(0x00, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
	put_string("Hello, World!", 0, 3 * CHAR_HEIGHT, get_color(0x05, 0x00, 0x00), get_color(0x00, 0x00, 0x00));
	put_string("Hello, World!", 0, 4 * CHAR_HEIGHT, get_color(0x05, 0x00, 0x05), get_color(0x00, 0x00, 0x00));
	put_string("Hello, World!", 0, 5 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x00), get_color(0x00, 0x00, 0x00));
	put_string("Hello, World!", 0, 6 * CHAR_HEIGHT, get_color(0x05, 0x05, 0x05), get_color(0x00, 0x00, 0x00));
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

