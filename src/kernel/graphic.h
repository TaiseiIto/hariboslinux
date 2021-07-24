#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
typedef struct
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} Color;

// fill box
// x and y can be negative
// 0 <= red   < 6
// 0 <= green < 6
// 0 <= blue  < 6
void fill_box(short x, short y, unsigned short width, unsigned short height, Color color);

// init screen
void init_screen(unsigned short screen_width, unsigned short screen_height);

// put character at screen(x, y)
void put_char(unsigned char character, unsigned short x, unsigned short y, Color foreground, Color background);

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
void put_dot(unsigned short x, unsigned short y, Color color);

// put string at screen(x, y)
void put_string(char const *string, unsigned short x, unsigned short y, Color foreground, Color background);

#endif

