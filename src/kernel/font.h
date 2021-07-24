#ifndef _FONT_H_
#define _FONT_H_

#define CHAR_WIDTH 0x08
#define CHAR_HEIGHT 0x10

// width 8 pixels
// height 16 pixels
typedef struct
{
	unsigned char row[0x10];
} CharFont;

extern CharFont const font[0x100];

// return value
// 0 means background color should be put at the pixel(x, y) of the character
// 1 means foreground color should be put at the pixel(x, y) of the character
unsigned char get_font_pixel(unsigned char character, unsigned char x, unsigned char y);

#endif

