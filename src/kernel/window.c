#include "memory.h"
#include "window.h"

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height)
{
	Color background_color;
	Window *new_window;
	background_color.red = 0xff;
	background_color.green = 0xff;
	background_color.blue = 0xff;
	background_color.alpha = 0xff;
	new_window = malloc(sizeof(*new_window));
	new_window->root_sheet = create_sheet(background_sheet, x, y, width, height);
	fill_box_sheet(new_window->root_sheet, 0, 0, width, height, background_color);
	return new_window;
}

