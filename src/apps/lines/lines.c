#include "dev.h"
#include "stdio.h"

int main(void)
{
	unsigned int window;
	unsigned short const window_width = 0x0200;
	unsigned short const window_height = 0x0200;
	short center_x = window_width / 2;
	short center_y = window_height / 2;
	short edge_x = 0;
	short edge_y = 0;
	unsigned char color_code = 1;
	#define COLOR_CODE_R 1
	#define COLOR_CODE_G 2
	#define COLOR_CODE_B 4
	Color color;
	color.red   = 0x00;
	color.green = 0x00;
	color.blue  = 0x00;
	color.alpha = 0xff;
	window = create_window("lines.com", 0x0200, 0x0200, window_width, window_height);
	do
	{
		unsigned char flags = 0;
		#define CHANGE_COLOR_CODE 0x01
		draw_line_window(window, center_x, center_y, edge_x, edge_y, color);
		if(edge_y == 0 && edge_x < window_width - 1)edge_x++;
		else if(edge_x == window_width - 1 && edge_y < window_height - 1)edge_y++;
		else if(edge_y == window_height - 1 && 0 < edge_x)edge_x--;
		else if(edge_x == 0 && 0 < edge_y)edge_y--;
		else break;
		switch(color_code & COLOR_CODE_R)
		{
		case 0:
			if(0 < color.red)
			{
				color.red--;
				if(color.red == 0)flags |= CHANGE_COLOR_CODE;
			}
			break;
		case COLOR_CODE_R:
			if(color.red < 0xff)
			{
				color.red++;
				if(color.red == 0xff)flags |= CHANGE_COLOR_CODE;
			}
			break;
		}
		switch(color_code & COLOR_CODE_G)
		{
		case 0:
			if(0 < color.green)
			{
				color.green--;
				if(color.green == 0)flags |= CHANGE_COLOR_CODE;
			}
			break;
		case COLOR_CODE_G:
			if(color.green < 0xff)
			{
				color.green++;
				if(color.green == 0xff)flags |= CHANGE_COLOR_CODE;
			}
			break;
		}
		switch(color_code & COLOR_CODE_B)
		{
		case 0:
			if(0 < color.blue)
			{
				color.blue--;
				if(color.blue == 0)flags |= CHANGE_COLOR_CODE;
			}
			break;
		case COLOR_CODE_B:
			if(color.blue < 0xff)
			{
				color.blue++;
				if(color.blue == 0xff)flags |= CHANGE_COLOR_CODE;
			}
			break;
		}
		if(flags & CHANGE_COLOR_CODE)color_code = (color_code + 1) % 8;
	} while(edge_x || edge_y);
	return 0;
}

