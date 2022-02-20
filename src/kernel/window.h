#ifndef _WINDOW_H_
#define _WINDOW_H_

struct _Window;

typedef struct _CloseButtonClickedEvent
{
	struct _Window *window;
} CloseButtonClickedEvent;

typedef struct _WindowDeletionResponseEvent
{
	struct _Window *window;
} WindowDeletionResponseEvent;

#include "sheet.h"

typedef struct _Window
{
	struct _Sheet *client_sheet;
	struct _Sheet *close_button_sheet;
	struct _Sheet *root_sheet;
	struct _Sheet *title_sheet;
	char *title;
	struct _Window *previous;
	struct _Window *next;
} Window;

Window *create_window(char *title, struct _Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height, Queue *event_queue);
void delete_window(Window *window, Queue *window_deletion_response_event_queue);

#endif

