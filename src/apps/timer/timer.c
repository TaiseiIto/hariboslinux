#include "dev.h"
#include "stdio.h"

int main(void)
{
	#define WINDOW_EXISTS 0x01
	unsigned char flags = WINDOW_EXISTS;
	unsigned int window = create_window("timer", 0x0200, 0x0200, 0x0200, 0x0200);
	create_timer(100, 100);
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
		case APPLICATION_EVENT_TYPE_NOTHING:
			hlt_application();
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			if(application_event.event_union.window_deletion_response_event.window == window)flags &= ~WINDOW_EXISTS;
			break;
		}
		process_event();
	}
	return 0;
}

