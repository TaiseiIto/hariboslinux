#ifndef _WINDOW_H_
#define _WINDOW_H_

struct _Window;

typedef struct _CloseButtonClickedEvent
{
	struct _Window *window;
} CloseButtonClickedEvent;

#include "sheet.h"

typedef struct _Window
{
	struct _Sheet *client_sheet;
	struct _Sheet *close_button_sheet;
	struct _Sheet *root_sheet;
	struct _Sheet *title_sheet;
	struct _Window *previous;
	struct _Window *next;
} Window;

Window *create_window(struct _Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height);
void delete_window(Window *window);

#endif

