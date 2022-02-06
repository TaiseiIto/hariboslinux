#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "sheet.h"

typedef struct _Window
{
	Sheet *client_sheet;
	Sheet *close_button_sheet;
	Sheet *root_sheet;
	Sheet *title_sheet;
	struct _Window *previous;
	struct _Window *next;
} Window;

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height);
void delete_window(Window *window);

#endif

