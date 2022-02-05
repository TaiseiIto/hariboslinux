#include "memory.h"
#include "serial.h"
#include "window.h"

#define EDGE_WIDTH 5
#define TITLE_SHEET_HEIGHT 0x12

//					//{red ,green, blue,alpha}
const Color background_color		= {0x80, 0x80, 0x80, 0xff};
const Color client_background_color	= {0x80, 0x80, 0x80, 0xff};
const Color close_button_cross_color	= {0x00, 0x00, 0x00, 0xff};
const Color dark_limit_color		= {0x00, 0x00, 0x00, 0xff};
const Color light_limit_color		= {0xff, 0xff, 0xff, 0xff};
const Color semi_dark_limit_color	= {0x40, 0x40, 0x40, 0xff};
const Color semi_light_limit_color	= {0xc0, 0xc0, 0xc0, 0xff};
const Color title_background_color	= {0x00, 0x00, 0x80, 0xff};

void *client_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event);
void *close_button_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event);
void *root_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event);
void *title_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event);

void *client_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event)
{
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		// Draw client sheet
		fill_box_sheet(sheet, 0, 0, sheet->width, sheet->height, client_background_color);
		return NULL;
	case EVENT_TYPE_SHEET_CLICKED:
	case EVENT_TYPE_SHEET_MOUSE_MOVE:
		return sheet->parent->event_procedure(sheet->parent, event);
	default:
		return default_event_procedure(sheet, event);
	}
}

void *close_button_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event)
{
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		// Draw close button sheet
		fill_box_sheet(sheet, 0, 0, sheet->width - 1, 1, light_limit_color);
		fill_box_sheet(sheet, 0, 1, 1, sheet->height - 2, light_limit_color);
		fill_box_sheet(sheet, 1, 1, sheet->width - 3, 1, semi_light_limit_color);
		fill_box_sheet(sheet, 1, 2, 1, sheet->height - 4, semi_light_limit_color);
		fill_box_sheet(sheet, 1, sheet->height - 2, sheet->width - 2, 1, semi_dark_limit_color);
		fill_box_sheet(sheet, sheet->width - 2, 1, 1, sheet->height - 3, semi_dark_limit_color);
		fill_box_sheet(sheet, 0, sheet->height - 1, sheet->width, 1, dark_limit_color);
		fill_box_sheet(sheet, sheet->width - 1, 0, 1, sheet->height - 1, dark_limit_color);
		for(unsigned short x_i = 2; x_i < sheet->width - 2; x_i++)for(unsigned short y_i = 2; y_i < sheet->height - 2; y_i++)
		{
			if(2 < x_i && x_i < sheet->width - 3 && 2 < y_i && y_i < sheet->height - 3 && (x_i - y_i <= 1 && y_i - x_i <= 1 || sheet->width - 2 <= x_i + y_i && x_i + y_i <= sheet->width))put_dot_sheet(sheet, x_i, y_i, close_button_cross_color);
			else put_dot_sheet(sheet, x_i, y_i, background_color);
		}
		return NULL;
	case EVENT_TYPE_SHEET_CLICKED:
		if(event->event_union.sheet_clicked_event.flags & SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON)
		{
			if(event->event_union.sheet_clicked_event.flags & SHEET_CLICKED_EVENT_FLAG_PUSHED)
			{
				// Close button is pushed.
				fill_box_sheet(sheet, 0, 0, sheet->width - 1, 1, dark_limit_color);
				fill_box_sheet(sheet, 0, 1, 1, sheet->height - 2, dark_limit_color);
				fill_box_sheet(sheet, 1, 1, sheet->width - 3, 1, semi_dark_limit_color);
				fill_box_sheet(sheet, 1, 2, 1, sheet->height - 4, semi_dark_limit_color);
				fill_box_sheet(sheet, 1, sheet->height - 2, sheet->width - 2, 1, semi_light_limit_color);
				fill_box_sheet(sheet, sheet->width - 2, 1, 1, sheet->height - 3, semi_light_limit_color);
				fill_box_sheet(sheet, 0, sheet->height - 1, sheet->width, 1, light_limit_color);
				fill_box_sheet(sheet, sheet->width - 1, 0, 1, sheet->height - 1, light_limit_color);
			}
			else if(event->event_union.sheet_clicked_event.flags & SHEET_CLICKED_EVENT_FLAG_RELEASED)
			{
				// Close button is released.
				fill_box_sheet(sheet, 0, 0, sheet->width - 1, 1, light_limit_color);
				fill_box_sheet(sheet, 0, 1, 1, sheet->height - 2, light_limit_color);
				fill_box_sheet(sheet, 1, 1, sheet->width - 3, 1, semi_light_limit_color);
				fill_box_sheet(sheet, 1, 2, 1, sheet->height - 4, semi_light_limit_color);
				fill_box_sheet(sheet, 1, sheet->height - 2, sheet->width - 2, 1, semi_dark_limit_color);
				fill_box_sheet(sheet, sheet->width - 2, 1, 1, sheet->height - 3, semi_dark_limit_color);
				fill_box_sheet(sheet, 0, sheet->height - 1, sheet->width, 1, dark_limit_color);
				fill_box_sheet(sheet, sheet->width - 1, 0, 1, sheet->height - 1, dark_limit_color);
			}
		}
		return default_event_procedure(sheet, event);
	case EVENT_TYPE_SHEET_MOUSE_MOVE:
		return default_event_procedure(sheet, event);
	default:
		return default_event_procedure(sheet, event);
	}
}

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height)
{
	Window *new_window;
	// Create sheets
	new_window = malloc(sizeof(*new_window));
	new_window->root_sheet = create_sheet(background_sheet, x, y, width, height, root_sheet_event_procedure);
	new_window->title_sheet = create_sheet(new_window->root_sheet, EDGE_WIDTH, EDGE_WIDTH, new_window->root_sheet->width - 2 * EDGE_WIDTH, TITLE_SHEET_HEIGHT, title_sheet_event_procedure);
	new_window->client_sheet = create_sheet(new_window->root_sheet, EDGE_WIDTH, new_window->title_sheet->y + new_window->title_sheet->height + EDGE_WIDTH, new_window->root_sheet->width - 2 * EDGE_WIDTH, new_window->root_sheet->height - new_window->title_sheet->y - new_window->title_sheet->height - 2 * EDGE_WIDTH, client_sheet_event_procedure);
	new_window->close_button_sheet = create_sheet(new_window->title_sheet, new_window->title_sheet->width - new_window->title_sheet->height + 1, 1, new_window->title_sheet->height - 2, new_window->title_sheet->height - 2, close_button_sheet_event_procedure);
	return new_window;
}

void *root_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event)
{
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		// Draw root sheet
		fill_box_sheet(sheet, 0, 0, sheet->width - 1, 1, light_limit_color);
		fill_box_sheet(sheet, 0, 1, 1, sheet->height - 2, light_limit_color);
		fill_box_sheet(sheet, 1, 1, sheet->width - 3, 1, semi_light_limit_color);
		fill_box_sheet(sheet, 1, 2, 1, sheet->height - 4, semi_light_limit_color);
		fill_box_sheet(sheet, 2, 2, sheet->width - 4, sheet->height - 4, background_color);
		fill_box_sheet(sheet, 1, sheet->height - 2, sheet->width - 2, 1, semi_dark_limit_color);
		fill_box_sheet(sheet, sheet->width - 2, 1, 1, sheet->height - 3, semi_dark_limit_color);
		fill_box_sheet(sheet, 0, sheet->height - 1, sheet->width, 1, dark_limit_color);
		fill_box_sheet(sheet, sheet->width - 1, 0, 1, sheet->height - 1, dark_limit_color);
		// Draw edges of children sheet of root sheet
		for(Sheet *root_child = sheet->lowest_child; root_child; root_child = root_child->upper)
		{
			fill_box_sheet(sheet, root_child->x - 2, root_child->y - 2, root_child->width + 3, 1, semi_dark_limit_color);
			fill_box_sheet(sheet, root_child->x - 2, root_child->y - 1, 1, root_child->height + 2, semi_dark_limit_color);
			fill_box_sheet(sheet, root_child->x - 1, root_child->y - 1, root_child->width + 1, 1, dark_limit_color);
			fill_box_sheet(sheet, root_child->x - 1, root_child->y, 1, root_child->height, dark_limit_color);
			fill_box_sheet(sheet, root_child->x - 1, root_child->y + root_child->height, root_child->width + 2, 1, light_limit_color);
			fill_box_sheet(sheet, root_child->x + root_child->width, root_child->y - 1, 1, root_child->height + 1, light_limit_color);
			fill_box_sheet(sheet, root_child->x - 2, root_child->y + root_child->height + 1, root_child->width + 4, 1, semi_light_limit_color);
			fill_box_sheet(sheet, root_child->x + root_child->width + 1, root_child->y - 2, 1, root_child->height + 3, semi_light_limit_color);
		}
		return NULL;
	default:
		return default_event_procedure(sheet, event);
	}
}

void *title_sheet_event_procedure(struct _Sheet *sheet, struct _Event const *event)
{
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		// Draw title sheet
		fill_box_sheet(sheet, 0, 0, sheet->width, sheet->height, title_background_color);
		return NULL;
	case EVENT_TYPE_SHEET_CLICKED:
	case EVENT_TYPE_SHEET_MOUSE_MOVE:
		return sheet->parent->event_procedure(sheet->parent, event);
	default:
		return default_event_procedure(sheet, event);
	}
}

