#include "dev.h"
#include "stdio.h"

int main(void)
{
	ApplicationEvent application_event;
	Color black;
	Color red;
	Color transparent;
	unsigned int window;
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	red.red = 0xff;
	red.green = 0x00;
	red.blue = 0x00;
	red.alpha = 0xff;
	transparent.red = 0x00;
	transparent.green = 0x00;
	transparent.blue = 0x00;
	transparent.alpha = 0x00;
	window = create_window("window.com", 0x0200, 0x0200, 0x0200, 0x0200);
	fill_box_window(window, 0x0010, 0x0010, 0x0010, 0x0010, red);
	print_window(window, 0x00, 0x00, black, transparent, "Hello, World!\n");
	put_dot_window(window, 0x0030, 0x0030, black);
	application_event = dequeue_application_event(window);
	switch(application_event.type)
	{
	case APPLICATION_EVENT_TYPE_NOTHING:
		printf("There is no event.\n");
		break;
	default:
		printf("Invalid event.\n");
		break;
	}
	return 0;
}

