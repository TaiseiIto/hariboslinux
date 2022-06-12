#include "dev.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

int main(void)
{
	Color black;
	unsigned int window;
	const unsigned short window_width  = 0x0200;
	const unsigned short window_height = 0x0200;
	srand(time(NULL));
	black.red = 0x00;
	black.green = 0x00;
	black.blue = 0x00;
	black.alpha = 0xff;
	window = create_window("stars", 0x0000, 0x0000, window_width, window_height);
	fill_box_window(window, 0x0000, 0x0000, window_width, window_height, black);
	for(unsigned int star = 0; star < 1000; star++)
	{
		Color star_color;
		star_color.red   = (unsigned char)(rand() % 0x00000100);
		star_color.green = (unsigned char)(rand() % 0x00000100);
		star_color.blue  = (unsigned char)(rand() % 0x00000100);
		star_color.alpha = 0xff;
		put_dot_window(window, (unsigned short)(rand() % (unsigned int)window_width), (unsigned short)(rand() % (unsigned int)window_height), star_color);
	}
	return 0;
}

