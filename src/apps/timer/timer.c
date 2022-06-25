#include "dev.h"
#include "stdio.h"

int main(void)
{
	Color black;
	Color transparent;
	#define WINDOW_EXISTS 0x01
	unsigned char flags = WINDOW_EXISTS;
	unsigned int window = create_window("timer", 0x0200, 0x0200, 0x0200, 0x0200);
	unsigned int time = 0;
	unsigned int timer;
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	transparent.red = 0x00;
	transparent.green = 0x00;
	transparent.blue = 0x00;
	transparent.alpha = 0x00;
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
		case APPLICATION_EVENT_TYPE_NOTHING:
			hlt_application();
			break;
		case APPLICATION_EVENT_TYPE_TIMER:
			if(application_event.event_union.timer_event.timer == timer)
			{
				printf_window(window, 0, 0, black, transparent, "%d", time);
				time++;
			}
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CREATED:
			timer = create_timer(0, 100);
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			if(application_event.event_union.window_deletion_response_event.window == window)flags &= ~WINDOW_EXISTS;
			break;
		}
		process_event();
	}
	return 0;
}

