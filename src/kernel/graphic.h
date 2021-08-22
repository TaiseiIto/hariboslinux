#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_

typedef unsigned char Color;

// fill box
// x and y can be negative
void fill_box(short x, short y, unsigned short width, unsigned short height, Color color);

// init screen
void init_screen(void);

// put character at screen(x, y)
void put_char(unsigned char character, unsigned short x, unsigned short y, Color foreground, Color background);

// put dot
// 0 <= x < screen width
// 0 <= y < screen height
void put_dot(unsigned short x, unsigned short y, Color color);

// put string at screen(x, y)
void put_string(char const *string, unsigned short x, unsigned short y, Color foreground, Color background);

// printf to screen
void printf_screen(unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...);

#endif

