#ifndef _TEXT_BOX_H_
#define _TEXT_BOX_H_

#include "sheet.h"

typedef struct _TextBox
{
	Sheet *sheet;
	void *(*default_event_procedure)(struct _Sheet *sheet, struct _Event const *event);
	Timer *cursor_blink_timer;
	struct _TextBox *next;
	struct _TextBox *previous;
	Color foreground_color;
	Color background_color;
} TextBox;

TextBox *make_sheet_text_box(Sheet *sheet, Color foreground_color, Color background_color);

#endif

