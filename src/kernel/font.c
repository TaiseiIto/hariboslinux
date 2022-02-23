#include "font.h"
#include "serial.h"

// return value
// 0 means background color should be put at the pixel(x, y) of the character
// 1 means foreground color should be put at the pixel(x, y) of the character
unsigned char get_font_pixel(unsigned char character, unsigned char x, unsigned char y)
{
	if(CHAR_WIDTH <= x)ERROR();
	if(CHAR_HEIGHT <= y)ERROR();
	return font[character].row[y] >> x & 0x01;
}

