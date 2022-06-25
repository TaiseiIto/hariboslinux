#include "dev.h"
#include "stdio.h"

int main(void)
{
	Color black;
	Color gray;
	#define WINDOW_EXISTS 0x01
	unsigned char flags = WINDOW_EXISTS;
	unsigned int window = create_window("timer", 0x0200, 0x0200, 10 * CHAR_WIDTH, 3 * CHAR_HEIGHT);
	unsigned int time = 0;
	unsigned int timer;
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	gray.red = 0x80;
	gray.green = 0x80;
	gray.blue = 0x80;
	gray.alpha = 0xff;
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
				unsigned int second = time % 60;
				unsigned int minute = (time / 60) % 60;
				unsigned int hour = time / 3600;
				printf_window(window, CHAR_WIDTH, CHAR_HEIGHT, black, gray, "%02d:%02d:%02d", hour, minute, second);
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

