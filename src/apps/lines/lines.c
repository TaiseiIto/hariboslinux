#include "dev.h"
#include "stdio.h"

int main(void)
{
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
	return 0;
}

