#ifndef _SHEET_H_
#define _SHEET_H_

struct _Sheet;

#include "event.h"
#include "graphic.h"

typedef struct _Sheet
{
	Color *image; // Color of point (x, y) is this->image[x + y * this->width], provided that 0 <= x < this->width and 0 <= y < this->height.
	Color *input; // Input image from lower sheet is needed to determine output color when alpha value is variable.
	Color *self_output; // output image from this.
	Color *family_output; // output image from this and its descendants.
	struct _Sheet *parent;
	struct _Sheet *upper;
	struct _Sheet *lower;
	struct _Sheet *uppest_child;
	struct _Sheet *lowest_child;
	void *(*event_procedure)(struct _Sheet *sheet, struct _Event const *event);
	short x, y; // Coordinate seen from parent
	unsigned short width, height;
} Sheet;

Sheet *create_sheet(Sheet *parent, short x, short y, unsigned short width, unsigned short height);
void delete_sheet(Sheet *sheet);
void fill_box_sheet(Sheet *sheet, short x, short y, unsigned short width, unsigned short height, Color color);
Sheet *get_uppest_sheet(Sheet *sheet, unsigned short x, unsigned short y);
void init_sheets(Sheet **_background_sheet, Sheet **_mouse_cursor_sheet);
void move_sheet(Sheet *sheet, short x, short y);
void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...);
void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *string);
void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character);
void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color);

#endif

