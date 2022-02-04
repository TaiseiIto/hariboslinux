#include "memory.h"
#include "window.h"

#define EDGE_WIDTH 5

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height)
{

	//					//{red ,green, blue,alpha}
	const Color background_color		= {0x80, 0x80, 0x80, 0xff};
	const Color dark_limit_color		= {0x00, 0x00, 0x00, 0xff};
	const Color light_limit_color		= {0xff, 0xff, 0xff, 0xff};
	const Color semi_dark_limit_color	= {0x40, 0x40, 0x40, 0xff};
	const Color semi_light_limit_color	= {0xc0, 0xc0, 0xc0, 0xff};
	const Color title_background_color	= {0x00, 0x00, 0x80, 0xff};
	Window *new_window;
	// Create sheets
	new_window = malloc(sizeof(*new_window));
	new_window->root_sheet = create_sheet(background_sheet, x, y, width, height);
	new_window->title_sheet = create_sheet(new_window->root_sheet, EDGE_WIDTH, EDGE_WIDTH, new_window->root_sheet->width - 2 * EDGE_WIDTH, 16);
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
	// Draw edges of children sheet of root sheet
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

