#ifndef _SHEET_H_
#define _SHEET_H_

#include"stdbool.h"

struct _Sheet;

typedef struct _SheetClickedEvent
{
	struct _Sheet *sheet;
	unsigned short x, y;
	unsigned char flags;
	#define SHEET_CLICKED_EVENT_FLAG_PUSHED		0x01
	#define SHEET_CLICKED_EVENT_FLAG_RELEASED	0x02
	#define SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON	0x04
	#define SHEET_CLICKED_EVENT_FLAG_MIDDLE_BUTTON	0x08
	#define SHEET_CLICKED_EVENT_FLAG_RIGHT_BUTTON	0x10
	#define SHEET_CLICKED_EVENT_FLAG_4TH_BUTTON	0x20
	#define SHEET_CLICKED_EVENT_FLAG_5TH_BUTTON	0x40
} SheetClickedEvent;

typedef struct _SheetCreatedEvent
{
	struct _Sheet *sheet;
} SheetCreatedEvent;

typedef struct _SheetMouseMoveEvent
{
	struct _Sheet *sheet;
	short x_movement, y_movement;
} SheetMouseMoveEvent;

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

Sheet *create_sheet(Sheet *parent, short x, short y, unsigned short width, unsigned short height, void *(*event_procedure)(struct _Sheet *sheet, struct _Event const *event));
void *default_event_procedure(Sheet *sheet, struct _Event const *event);
void delete_sheet(Sheet *sheet);
void fill_box_sheet(Sheet *sheet, short x, short y, unsigned short width, unsigned short height, Color color);
short get_sheet_x_on_screen(Sheet const *sheet);
short get_sheet_y_on_screen(Sheet const *sheet);
Sheet *get_uppest_sheet(Sheet *sheet, unsigned short x, unsigned short y);
void init_sheets(Sheet **_background_sheet, Sheet **_mouse_cursor_sheet);
bool is_descendant_sheet_of(Sheet const *descendant_candidate, Sheet const *ancestor_candidate);
void move_sheet(Sheet *sheet, short x, short y);
void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...);
void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *string);
void pull_up_sheet(Sheet *sheet);
void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character);
void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color);
void send_sheets_event(struct _Event const *event);

#endif

