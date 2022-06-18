#include "dev.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(void)
{
	Color black;
	#define WINDOW_EXISTS 0x01
	unsigned char flags = WINDOW_EXISTS;
	unsigned int window;
	const unsigned short window_width  = 0x0200;
	const unsigned short window_height = 0x0200;
	srand(time(NULL));
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	window = create_window("stars", 0x0000, 0x0000, window_width, window_height);
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
		case APPLICATION_EVENT_TYPE_NOTHING:
			hlt_application();
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CREATED:
			fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
			for(unsigned int star = 0; star < 0x0400; star++)
			{
				Color star_color;
				star_color.red   = (unsigned char)(rand() % 0x00000100);
				star_color.green = (unsigned char)(rand() % 0x00000100);
				star_color.blue  = (unsigned char)(rand() % 0x00000100);
				star_color.alpha = 0xff;
				put_dot_window(window, (unsigned short)(rand() % (unsigned int)window_width), (unsigned short)(rand() % (unsigned int)window_height), star_color);
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			if(application_event.event_union.window_deletion_response_event.window == window)flags &= ~WINDOW_EXISTS;
			break;
		default:
			ERROR(); // Invalid event type.
			break;
		}
		process_event();
	}
	return 0;
}

