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
	Color color;
	Color foreground;
	Color background;
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
	for(unsigned short y = 0; y < boot_information.screen_height; y++)for(unsigned short x = 0; x < boot_information.screen_width; x++)
	{
		color.red = x % 0x06;
		color.green = y % 0x06;
		color.blue = 0x00;
		put_dot(x, y, color);
	}
	color.red = 0x05;
	color.green = 0x00;
	color.blue = 0x00;
	fill_box(0x0000, 0x0000, 0x0010, 0x0010, color);
	color.red = 0x00;
	color.green = 0x05;
	color.blue = 0x00;
	fill_box(0x0010, 0x0000, 0x0010, 0x0010, color);
	color.red = 0x00;
	color.green = 0x00;
	color.blue = 0x05;
	fill_box(0x0020, 0x0000, 0x0010, 0x0010, color);
	foreground.red + 0x05;
	foreground.green + 0x05;
	foreground.blue + 0x05;
	background.red + 0x00;
	background.green + 0x00;
	background.blue + 0x00;
	put_char('A', 0, 0, foreground, background);
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

