#include "io.h"
#include "segment.h"
#include "serial.h"


typedef struct
{
	unsigned short width;
	unsigned short height;
} ScreenInformation;

ScreenInformation screen_information;

// 1 byte per pixel
// 3 primary colors(RGB)
// 6 brightnesses (hardware constraints 0x00~0x3f)
// 216 colors
// red brightnesses:   0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// green brightnesses: 0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// blue brightnesses:  0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
void init_palette(void);

// 1 byte per pixel
// 3 primary colors(RGB)
// 6 brightnesses  (hardware constraints 0x00~0x3f)
// 216 colors
// red brightnesses:   0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// green brightnesses: 0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
// blue brightnesses:  0x00, 0x0c, 0x18, 0x24, 0x30, 0x3c
void init_palette(void)
{
	unsigned int eflags = get_eflags();
	if(eflags & INTERRUPT_FLAG)cli();
	outb(0x03c8, 0x00);
	for(unsigned short color_number = 0x0000; color_number < 0x0100; color_number++)
	{
		if(color_number < 0x06 * 0x06 * 0x06)
		{
			unsigned char color_number_destroyable;
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			color_number_destroyable = (unsigned char)color_number;
			red = 0x0c * (color_number_destroyable % 6);
			color_number_destroyable /= 6;
			green = 0x0c * (color_number_destroyable % 6);
			color_number_destroyable /= 6;
			blue = 0x0c * (color_number_destroyable % 6);
			outb(0x03c9, red);
			outb(0x03c9, green);
			outb(0x03c9, blue);
		}
		else
		{
			outb(0x03c9, 0x00);
			outb(0x03c9, 0x00);
			outb(0x03c9, 0x00);
		}
	}
	if(eflags & INTERRUPT_FLAG)sti();
}

// init screen
void init_screen(unsigned short screen_width, unsigned short screen_height)
{
	screen_information.width = screen_width;
	screen_information.height = screen_height;
	print_serial_polling("screen_information.width = 0x");
	print_word_hex_serial_polling(screen_information.width);
	new_line_serial_polling();
	print_serial_polling("screen_information.height = 0x");
	print_word_hex_serial_polling(screen_information.height);
	new_line_serial_polling();
	init_palette();
}

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
void put_dot(unsigned short x, unsigned short y, unsigned char red, unsigned char green, unsigned char blue)
{
	if(screen_information.width <= x)return;
	if(screen_information.height <= y)return;
	if(0x06 <= red)return;
	if(0x06 <= green)return;
	if(0x06 <= blue)return;
	writeb(VRAM_SEGMENT, (void *)(screen_information.width * y + x), 6 * (6 * blue + green) + red);
}

