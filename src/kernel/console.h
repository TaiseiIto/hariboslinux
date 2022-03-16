#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "text_box.h"

typedef struct _Console
{
	TextBox *text_box;
	void *(*default_event_procedure)(struct _Sheet *sheet, struct _Event const *event);
	struct _Console *next;
	struct _Console *previous;
} Console;

Console *make_sheet_console(Sheet *sheet, Color foreground_color, Color background_color);

#endif

