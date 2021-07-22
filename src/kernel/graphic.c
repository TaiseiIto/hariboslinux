#include "io.h"

// 1 byte per pixel
// 3 primary colors(RGB)
// 6 brightnesses 
// 216 colors
// red brightnesses:   0x00, 0x33, 0x66, 0x99, 0xcc, 0xff
// green brightnesses: 0x00, 0x33, 0x66, 0x99, 0xcc, 0xff
// blue brightnesses:  0x00, 0x33, 0x66, 0x99, 0xcc, 0xff
void init_palette(void)
{
	unsigned int eflags = get_eflags();
	if(eflags & INTERRUPT_FLAG)cli();
	outb(0x03c8, 0x00);
	for(unsigned char color_number = 0x00; color_number < 0x06 * 0x06 * 0x06; color_number++)
	{
		unsigned char color_number_destroyable;
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		color_number_destroyable = color_number;
		red = 0x33 * (color_number_destroyable % 6);
		color_number_destroyable /= 6;
		green = 0x33 * (color_number_destroyable % 6);
		color_number_destroyable /= 6;
		blue = 0x33 * (color_number_destroyable % 6);
		outb(0x03c9, red);
		outb(0x03c9, green);
		outb(0x03c9, blue);
	}
	if(eflags & INTERRUPT_FLAG)sti();
}

