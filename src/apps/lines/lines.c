#include "dev.h"
#include "stdio.h"

int main(void)
{
	Color red;
	unsigned int window;
	window = create_window("lines.com", 0x0200, 0x0200, 0x0200, 0x0200);
	red.red   = 0xff;
	red.green = 0x00;
	red.blue  = 0x00;
	red.alpha = 0xff;
	draw_line_window(window, 0x0080, 0x0080, 0x0180, 0x0180, red);
	return 0;
}

