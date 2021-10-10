#ifndef _SHEET_H_
#define _SHEET_H_

#include "graphic.h"

typedef struct _Sheet
{
	Color *image;
	short x, y;
	unsigned short width, height;
	struct _Sheet *upper_sheet;
	struct _Sheet *lower_sheet;
} Sheet;

Sheet *create_sheet(short x, short y, unsigned short width, unsigned short height);
void delete_sheet(Sheet *sheet);
void fill_box_sheet(Sheet *sheet, short x, short y, unsigned short width, unsigned short height, Color color);
void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...);
void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *string);
void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character);
void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color);
void refresh_screen(void);

#endif

