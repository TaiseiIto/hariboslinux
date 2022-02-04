#include "memory.h"
#include "window.h"

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height)
{
	Color background_color;
	Color dark_limit_color;
	Color light_limit_color;
	Color semi_dark_limit_color;
	Color semi_light_limit_color;
	Color title_background_color;
	Window *new_window;
	// Define colors
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
	title_background_color.red = 0x00;
	title_background_color.green = 0x00;
	title_background_color.blue = 0xff;
	title_background_color.alpha = 0xff;
	// Create sheets
	new_window = malloc(sizeof(*new_window));
	new_window->root_sheet = create_sheet(background_sheet, x, y, width, height);
	new_window->title_sheet = create_sheet(new_window->root_sheet, 6, 6, new_window->root_sheet->width - 12, 16);
	// Draw root sheet
	fill_box_sheet(new_window->root_sheet, 0, 0, new_window->root_sheet->width - 1, 1, light_limit_color);
	fill_box_sheet(new_window->root_sheet, 0, 1, 1, new_window->root_sheet->height - 2, light_limit_color);
	fill_box_sheet(new_window->root_sheet, 1, 1, new_window->root_sheet->width - 3, 1, semi_light_limit_color);
	fill_box_sheet(new_window->root_sheet, 1, 2, 1, new_window->root_sheet->height - 4, semi_light_limit_color);
	fill_box_sheet(new_window->root_sheet, 2, 2, new_window->root_sheet->width - 4, new_window->root_sheet->height - 4, background_color);
	fill_box_sheet(new_window->root_sheet, 1, new_window->root_sheet->height - 2, new_window->root_sheet->width - 2, 1, semi_dark_limit_color);
	fill_box_sheet(new_window->root_sheet, new_window->root_sheet->width - 2, 1, 1, new_window->root_sheet->height - 3, semi_dark_limit_color);
	fill_box_sheet(new_window->root_sheet, 0, new_window->root_sheet->height - 1, new_window->root_sheet->width, 1, dark_limit_color);
	fill_box_sheet(new_window->root_sheet, new_window->root_sheet->width - 1, 0, 1, new_window->root_sheet->height - 1, dark_limit_color);
	for(Sheet *root_child = new_window->root_sheet->lowest_child; root_child; root_child = root_child->upper)
	{
		fill_box_sheet(new_window->root_sheet, root_child->x - 2, root_child->y - 2, root_child->width + 3, 1, semi_dark_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x - 2, root_child->y - 1, 1, root_child->height + 2, semi_dark_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x - 1, root_child->y - 1, root_child->width + 1, 1, dark_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x - 1, root_child->y, 1, root_child->height, dark_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x - 1, root_child->y + root_child->height, root_child->width + 2, 1, light_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x + root_child->width, root_child->y - 1, 1, root_child->height + 1, light_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x - 2, root_child->y + root_child->height + 1, root_child->width + 4, 1, semi_light_limit_color);
		fill_box_sheet(new_window->root_sheet, root_child->x + root_child->width + 1, root_child->y - 2, 1, root_child->height + 3, semi_light_limit_color);
	}
	// Draw title sheet
	fill_box_sheet(new_window->title_sheet, 0, 0, new_window->title_sheet->width, new_window->title_sheet->height, title_background_color);
	return new_window;
}

