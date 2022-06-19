#ifndef _WINDOW_H_
#define _WINDOW_H_

#define EDGE_WIDTH 5
#define TITLE_SHEET_HEIGHT 0x12

struct _Window;

typedef struct _CloseButtonClickedEvent
{
	struct _Window *window;
} CloseButtonClickedEvent;

typedef struct _WindowDeletionRequestEvent
{
	struct _Window *window;
} WindowDeletionRequestEvent;

typedef struct _WindowDeletionResponseEvent
{
	struct _Window *window;
} WindowDeletionResponseEvent;

typedef struct _WindowFocusedEvent
{
	struct _Window *window;
} WindowFocusedEvent;

typedef struct _WindowUnfocusedEvent
{
	struct _Window *window;
} WindowUnfocusedEvent;

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

void change_window_event_queue(Window *window, Queue *new_event_queue);
Window *create_window(char const *title, struct _Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height, Queue *event_queue);
void delete_window(Window *window, Queue *window_deletion_response_event_queue);
void focus_window(Window *window);
Window *get_window_from_sheet(struct _Sheet const *sheet);

#endif

