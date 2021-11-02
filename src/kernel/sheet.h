#ifndef _SHEET_H_
#define _SHEET_H_

#include "graphic.h"

typedef struct _Sheet
{
	Color *image;
	Color *background; // Input image from lower sheet is needed to determine output color when alpha value is variable.
	short x, y;
	unsigned short width, height;
	struct _Sheet *upper_sheet;
	struct _Sheet *lower_sheet;
} Sheet;

Color ask_color_screen(Sheet *sheet, unsigned short x, unsigned short y);
Sheet *create_sheet(short x, short y, unsigned short width, unsigned short height);
void delete_sheet(Sheet *sheet);
void fill_box_sheet(Sheet *sheet, short x, short y, unsigned short width, unsigned short height, Color color);
Color get_color_screen(unsigned short x, unsigned short y);
void init_sheets(Sheet **_background_sheet, Sheet **_mouse_cursor_sheet);
void move_sheet(Sheet *sheet, short x, short y);
void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...);
void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *string);
void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character);
void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color);
void refresh_dot(unsigned short x, unsigned short y);
void refresh_rectangle(short x, short y, unsigned short width, unsigned short height);
void transmit_color_to_upper_sheet(Sheet *upper_sheet, unsigned short x, unsigned short y, Color color);

#endif

