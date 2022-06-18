#include "dev.h"
#include "stdio.h"

int main(void)
{
	#define WINDOW_EXISTS 0x01
	unsigned char flags = WINDOW_EXISTS;
	unsigned int window = create_window("window.com", 0x0200, 0x0200, 0x0200, 0x0200);
	while(flags & WINDOW_EXISTS)
	{
		ApplicationEvent application_event = dequeue_application_event();
		switch(application_event.type)
		{
		case APPLICATION_EVENT_TYPE_NOTHING:
			break;
		case APPLICATION_EVENT_TYPE_WINDOW_CREATED:
			printf("A window is created!\n");
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

