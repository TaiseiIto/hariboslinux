#include "dev.h"
#include "stdio.h"

int main(void)
{
	Color red;
	unsigned int window;
	red.red = 0xff;
	red.green = 0x00;
	red.blue = 0x00;
	red.alpha = 0xff;
	window = create_window("window.com", 0x0200, 0x0200, 0x0200, 0x0200);
	fill_box_window(window, 0x0010, 0x0010, 0x0010, 0x0010, red);
	return 0;
}

