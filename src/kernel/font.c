#include "font.h"
#include "serial.h"

// return value
// 0 means background color should be put at the pixel(x, y) of the character
// 1 means foreground color should be put at the pixel(x, y) of the character
unsigned char get_font_pixel(unsigned char character, unsigned char x, unsigned char y)
{
	print_serial_polling("font[");
	putchar_serial_polling(character);
	print_serial_polling("].row[");
	print_byte_hex_serial_polling(y);
	print_serial_polling("] = 0x");
	print_byte_hex_serial_polling(font[character].row[y]);
	new_line_serial_polling();
	return font[character].row[y] >> x & 0x01;
}

