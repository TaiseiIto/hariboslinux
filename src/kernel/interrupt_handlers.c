#include "font.h"
#include "graphic.h"
#include "serial.h"

void interrupt_handler0x0f(void)
{
	Color background_color;
	Color foreground_color;
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "int 0x0f");
	print_serial_polling("int 0x0f\n");
}

