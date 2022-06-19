#include "dev.h"
#include "stdio.h"

int main(void)
{
	Color black;
	Color white;
	short const window_x = 0x0200;
	short const window_y = 0x0200;
	short const window_width = 0x0200;
	short const window_height = 0x0200;
	unsigned int window = create_window("draw", window_x, window_y, window_width, window_height);
	short mouse_x, mouse_y;
	short next_mouse_x, next_mouse_y;
	#define WINDOW_EXISTS 0x01
	#define DRAGGED 0x02
	unsigned char flags = WINDOW_EXISTS;
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	white.red = 0xff;
	white.green = 0xff;
	white.blue = 0xff;
	white.alpha = 0xff;
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
		case APPLICATION_EVENT_TYPE_NOTHING:
			hlt_application();
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CLICKED:
			next_mouse_x = application_event.event_union.window_clicked_event.x;
			next_mouse_y = application_event.event_union.window_clicked_event.y;
			if(application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_PUSHED)flags |= DRAGGED;
			if(flags & DRAGGED && application_event.event_union.window_clicked_event.flags & APPLICATION_WINDOW_CLICKED_EVENT_FLAG_RELEASED)
			{
				draw_line_window(window, mouse_x, mouse_y, next_mouse_x, next_mouse_y, black);
				flags &= ~DRAGGED;
			}
			mouse_x = next_mouse_x;
			mouse_y = next_mouse_y;
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CREATED:
			fill_box_window(window, 0, 0, window_width, window_height, white);
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			if(application_event.event_union.window_deletion_response_event.window == window)flags &= ~WINDOW_EXISTS;
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DRAG:
			next_mouse_x = mouse_x + application_event.event_union.window_drag_event.x_movement;
			next_mouse_y = mouse_y + application_event.event_union.window_drag_event.y_movement;
			draw_line_window(window, mouse_x, mouse_y, next_mouse_x, next_mouse_y, black);
			mouse_x = next_mouse_x;
			mouse_y = next_mouse_y;
			flags |= DRAGGED;
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_MOVE:
			mouse_x += application_event.event_union.window_move_event.x_movement;
			mouse_y += application_event.event_union.window_move_event.y_movement;
			flags &= ~DRAGGED;
			break;
		}
		process_event();
	}
	return 0;
}

