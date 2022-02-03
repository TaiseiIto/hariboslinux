#include "memory.h"
#include "window.h"

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height)
{
	Color background_color;
	Color dark_limit_color;
	Color light_limit_color;
	Color semi_dark_limit_color;
	Color semi_light_limit_color;
	Window *new_window;
	background_color.red = 0x80;
	background_color.green = 0x80;
	background_color.blue = 0x80;
	background_color.alpha = 0xff;
	dark_limit_color.red = 0x20;
	dark_limit_color.green = 0x20;
	dark_limit_color.blue = 0x20;
	dark_limit_color.alpha = 0xff;
	light_limit_color.red = 0xe0;
	light_limit_color.green = 0xe0;
	light_limit_color.blue = 0xe0;
	light_limit_color.alpha = 0xff;
	semi_dark_limit_color.red = 0x40;
	semi_dark_limit_color.green = 0x40;
	semi_dark_limit_color.blue = 0x40;
	semi_dark_limit_color.alpha = 0xff;
	semi_light_limit_color.red = 0xc0;
	semi_light_limit_color.green = 0xc0;
	semi_light_limit_color.blue = 0xc0;
	semi_light_limit_color.alpha = 0xff;
	new_window = malloc(sizeof(*new_window));
	new_window->root_sheet = create_sheet(background_sheet, x, y, width, height);
	fill_box_sheet(new_window->root_sheet, 0, 0, width - 1, 1, light_limit_color);
	fill_box_sheet(new_window->root_sheet, 0, 1, 1, height - 2, light_limit_color);
	fill_box_sheet(new_window->root_sheet, 1, 1, width - 3, 1, semi_light_limit_color);
	fill_box_sheet(new_window->root_sheet, 1, 2, 1, height - 4, semi_light_limit_color);
	fill_box_sheet(new_window->root_sheet, 2, 2, width - 4, height - 4, background_color);
	fill_box_sheet(new_window->root_sheet, 1, height - 2, width - 2, 1, semi_dark_limit_color);
	fill_box_sheet(new_window->root_sheet, width - 2, 1, 1, height - 3, semi_dark_limit_color);
	fill_box_sheet(new_window->root_sheet, 0, height - 1, width, 1, dark_limit_color);
	fill_box_sheet(new_window->root_sheet, width - 1, 0, 1, height - 1, dark_limit_color);
	return new_window;
}

