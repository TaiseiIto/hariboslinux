#include "chain_string.h"
#include "font.h"
#include "io.h"
#include "memory.h"
#include "mouse_cursor.h"
#include "serial.h"
#include "sheet.h"
#include "stdio.h"
#include "task.h"

//			        {red ,green, blue,alpha}
const Color color_black	      = {0x00, 0x00, 0x00, 0xff};
const Color color_transparent = {0x00, 0x00, 0x00, 0x00};

Sheet *background_sheet = NULL;
Sheet *focused_sheet = NULL;
Sheet *mouse_cursor_sheet = NULL;

Color alpha_blend(Color foreground, Color background);
void print_sheet_tree(void);
void print_sheet_tree_level(Sheet *sheet, unsigned int level);
void refresh_input(Sheet *sheet); // refresh sheet->input.
void refresh_input_dot(Sheet *sheet, unsigned short x, unsigned short y); // refresh sheet->input[x + y * sheet->width].
void refresh_self_output(Sheet *sheet); // refresh sheet->self_output.
void refresh_self_output_dot(Sheet *sheet, unsigned short x, unsigned short y); // refresh sheet->self_output[x + y * sheet->width].
bool sheet_exists_under(Sheet const *sheet, Sheet const *root);
void transmit_family_output_dot(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->family_output[x + y * sheet->width].
void transmit_family_output_dot_through_opaques(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->family_output[x + y * sheet->width] through opaque sheets.
void transmit_self_input(Sheet *sheet); // transmit image sheet->self_input.
void transmit_self_input_dot(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->self_input[x + y * sheet->width].
void transmit_self_input_rectangle(Sheet *sheet, unsigned short x, unsigned short y, unsigned short width, unsigned short height); // transmit color sheet->self_input[x + y * sheet->width].
void transmit_self_output(Sheet *sheet); // transmit image sheet->self_output.
void transmit_self_output_dot(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->self_output[x + y * sheet->width].
void transmit_self_output_dot_through_opaques(Sheet *sheet, unsigned short x, unsigned short y); // transmit color sheet->self_output[x + y * sheet->width] through opaque sheets.
void transmit_self_output_rectangle(Sheet *sheet, unsigned short x, unsigned short y, unsigned short width, unsigned short height); // transmit color sheet->self_output[x + y * sheet->width].
void transmit_self_output_through_opaques(Sheet *sheet); // transmit image sheet->self_output through opaque sheets.

Color alpha_blend(Color foreground, Color background)
{
	Color blended;
	blended.red = (unsigned char)(((unsigned short)foreground.alpha * (unsigned short)foreground.red + (0x00ff - (unsigned short)foreground.alpha) * (unsigned short)background.red) >> 8);
	blended.green = (unsigned char)(((unsigned short)foreground.alpha * (unsigned short)foreground.green + (0x00ff - (unsigned short)foreground.alpha) * (unsigned short)background.green) >> 8);
	blended.blue = (unsigned char)(((unsigned short)foreground.alpha * (unsigned short)foreground.blue + (0x00ff - (unsigned short)foreground.alpha) * (unsigned short)background.blue) >> 8);
	blended.alpha = 0xff;
	return blended;
}

void change_sheet_event_queue(Sheet *sheet, Queue *new_event_queue)
{
	sheet->event_queue = new_event_queue;
	for(Sheet *child = sheet->lowest_child; child; child = child->upper)change_sheet_event_queue(child, new_event_queue);
}

Sheet *create_sheet(Sheet *parent, short x, short y, unsigned short width, unsigned short height, void *(*event_procedure)(struct _Sheet *sheet, struct _Event const *event), Queue *event_queue)
{
	Event sheet_created_event;
	Sheet *new_sheet = malloc(sizeof(*new_sheet));
	new_sheet->image = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->input = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->self_output = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->family_output = malloc(width * height * sizeof(*new_sheet->image));
	new_sheet->x = x;
	new_sheet->y = y;
	new_sheet->width = width;
	new_sheet->height = height;
	new_sheet->uppest_child = NULL;
	new_sheet->lowest_child = NULL;
	new_sheet->event_procedure = event_procedure ? event_procedure : default_event_procedure;
	new_sheet->event_queue = event_queue;
	new_sheet->flags = 0;
	prohibit_switch_task();
	new_sheet->parent = parent;
	if(parent)
	{
		if(parent->uppest_child && parent->lowest_child)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = parent->uppest_child;
			parent->uppest_child->upper = new_sheet;
			parent->uppest_child = new_sheet;
		}
		else if(!parent->uppest_child && !parent->lowest_child)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = NULL;
			parent->uppest_child = new_sheet;
			parent->lowest_child = new_sheet;
		}
		else ERROR();
	}
	else
	{
		if(background_sheet && mouse_cursor_sheet && background_sheet == mouse_cursor_sheet)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = mouse_cursor_sheet;
			mouse_cursor_sheet->upper = new_sheet;
			mouse_cursor_sheet = new_sheet;
		}
		else if(!background_sheet && !mouse_cursor_sheet)
		{
			new_sheet->upper = NULL;
			new_sheet->lower = NULL;
			background_sheet = new_sheet;
			mouse_cursor_sheet = new_sheet;
		}
		else new_sheet->parent = background_sheet;
	}
	allow_switch_task();
	refresh_input(new_sheet);
	sheet_created_event.type = EVENT_TYPE_SHEET_CREATED;
	sheet_created_event.event_union.sheet_created_event.sheet = new_sheet;
	enqueue(new_sheet->event_queue, &sheet_created_event);
	return new_sheet;
}

void *default_event_procedure(Sheet *sheet, Event const *event)
{
	Event new_event;
	prohibit_switch_task();
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CLICKED:
		if(event->event_union.sheet_clicked_event.flags == (SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON | SHEET_CLICKED_EVENT_FLAG_PUSHED))
		{
			for(Sheet *pulled_up_sheet = sheet; pulled_up_sheet && pulled_up_sheet != background_sheet; pulled_up_sheet = pulled_up_sheet->parent)pull_up_sheet(pulled_up_sheet);
		}
		if(event->event_union.sheet_clicked_event.flags == (SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON | SHEET_CLICKED_EVENT_FLAG_RELEASED))
		{
		}
		if(event->event_union.sheet_clicked_event.flags & SHEET_CLICKED_EVENT_FLAG_PUSHED)focus_sheet(sheet);
		break;
	case EVENT_TYPE_SHEET_CREATED:
		focus_sheet(sheet);
		break;
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		sheet->flags |= SHEET_FLAG_RECEIVED_DELETION_REQUEST;
		for(Sheet *child = sheet->uppest_child; child; child = child->lower)
		{
			new_event.type = EVENT_TYPE_SHEET_DELETION_REQUEST;
			new_event.event_union.sheet_deletion_request_event.sheet = child;
			enqueue(child->event_queue, &new_event);
		}
		if(!sheet->lowest_child)
		{
			new_event.type = EVENT_TYPE_SHEET_DELETION_RESPONSE;
			new_event.event_union.sheet_deletion_response_event.sheet = sheet;
			new_event.event_union.sheet_deletion_response_event.parent = sheet->parent;
			delete_sheet(sheet);
			enqueue(sheet->parent->event_queue, &new_event);
		}
		break;
	case EVENT_TYPE_SHEET_DELETION_RESPONSE:
		if(sheet->flags & SHEET_FLAG_RECEIVED_DELETION_REQUEST && !sheet->lowest_child)
		{
			new_event.type = EVENT_TYPE_SHEET_DELETION_RESPONSE;
			new_event.event_union.sheet_deletion_response_event.sheet = sheet;
			new_event.event_union.sheet_deletion_response_event.parent = sheet->parent;
			delete_sheet(sheet);
			enqueue(sheet->parent->event_queue, &new_event);
		}
		break;
	case EVENT_TYPE_SHEET_FOCUSED:
		break;
	case EVENT_TYPE_SHEET_KEYBOARD:
		break;
	case EVENT_TYPE_SHEET_MOUSE_DRAG:
		move_sheet(sheet, sheet->x + event->event_union.sheet_mouse_drag_event.x_movement, sheet->y + event->event_union.sheet_mouse_drag_event.y_movement);
		break;
	case EVENT_TYPE_SHEET_MOUSE_MOVE:
		break;
	case EVENT_TYPE_SHEET_UNFOCUSED:
		break;
	case EVENT_TYPE_SHEET_USER_DEFINED:
		break;
	case EVENT_TYPE_SHEET_VERTICAL_WHEEL:
		break;
	default:
		ERROR(); // Event that procedure is not defined.
		break;
	}
	allow_switch_task();
	return NULL;
}

void delete_sheet(Sheet *sheet)
{
	prohibit_switch_task();
	if(sheet == background_sheet || sheet == mouse_cursor_sheet)ERROR();
	while(sheet->uppest_child && sheet->lowest_child)delete_sheet(sheet->uppest_child);
	if(focused_sheet == sheet)focused_sheet = NULL;
	transmit_self_input(sheet);
	if(sheet->uppest_child || sheet->lowest_child)ERROR();
	if(sheet->parent)
	{
		if(sheet == sheet->parent->uppest_child)sheet->parent->uppest_child = sheet->lower;
		if(sheet == sheet->parent->lowest_child)sheet->parent->lowest_child = sheet->upper;
	}
	if(sheet->upper)sheet->upper->lower = sheet->lower;
	if(sheet->lower)sheet->lower->upper = sheet->upper;
	allow_switch_task();
	focus_sheet(sheet->parent);
	free(sheet->image);
	free(sheet->input);
	free(sheet->self_output);
	free(sheet->family_output);
	free(sheet);
}

void distribute_event(struct _Event const *event)
{
	prohibit_switch_task();
	switch(event->type)
	{
	case EVENT_TYPE_CLOSE_BUTTON_CLICKED:
		if(sheet_exists(event->event_union.close_button_clicked_event.window->root_sheet))event->event_union.close_button_clicked_event.window->root_sheet->event_procedure(event->event_union.close_button_clicked_event.window->root_sheet, event);
		break;
	case EVENT_TYPE_SHEET_CLICKED:
		if(sheet_exists(event->event_union.sheet_clicked_event.sheet))event->event_union.sheet_clicked_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_CREATED:
		if(sheet_exists(event->event_union.sheet_created_event.sheet))event->event_union.sheet_created_event.sheet->event_procedure(event->event_union.sheet_created_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		if(sheet_exists(event->event_union.sheet_deletion_request_event.sheet))event->event_union.sheet_deletion_request_event.sheet->event_procedure(event->event_union.sheet_deletion_request_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_DELETION_RESPONSE:
		if(sheet_exists(event->event_union.sheet_deletion_response_event.parent))event->event_union.sheet_deletion_response_event.parent->event_procedure(event->event_union.sheet_deletion_response_event.parent, event);
		break;
	case EVENT_TYPE_SHEET_FOCUSED:
		if(sheet_exists(event->event_union.sheet_focused_event.sheet))event->event_union.sheet_focused_event.sheet->event_procedure(event->event_union.sheet_focused_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_KEYBOARD:
		if(sheet_exists(event->event_union.sheet_keyboard_event.sheet))event->event_union.sheet_keyboard_event.sheet->event_procedure(event->event_union.sheet_keyboard_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_MOUSE_DRAG:
		if(sheet_exists(event->event_union.sheet_mouse_drag_event.sheet))event->event_union.sheet_mouse_drag_event.sheet->event_procedure(event->event_union.sheet_mouse_drag_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_MOUSE_MOVE:
		if(sheet_exists(event->event_union.sheet_mouse_move_event.sheet))event->event_union.sheet_mouse_move_event.sheet->event_procedure(event->event_union.sheet_mouse_move_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_UNFOCUSED:
		if(sheet_exists(event->event_union.sheet_unfocused_event.sheet))event->event_union.sheet_unfocused_event.sheet->event_procedure(event->event_union.sheet_unfocused_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_USER_DEFINED:
		if(sheet_exists(event->event_union.sheet_user_defined_event.sheet))event->event_union.sheet_user_defined_event.sheet->event_procedure(event->event_union.sheet_user_defined_event.sheet, event);
		break;
	case EVENT_TYPE_SHEET_VERTICAL_WHEEL:
		if(sheet_exists(event->event_union.sheet_vertical_wheel_event.sheet))event->event_union.sheet_vertical_wheel_event.sheet->event_procedure(event->event_union.sheet_vertical_wheel_event.sheet, event);
		break;
	case EVENT_TYPE_WINDOW_DELETION_REQUEST:
		if(sheet_exists(event->event_union.window_deletion_request_event.window->root_sheet))event->event_union.window_deletion_request_event.window->root_sheet->event_procedure(event->event_union.window_deletion_request_event.window->root_sheet, event);
		break;
	case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
		break;
	case EVENT_TYPE_WINDOW_FOCUSED:
		if(sheet_exists(event->event_union.window_focused_event.window->root_sheet))event->event_union.window_focused_event.window->root_sheet->event_procedure(event->event_union.window_focused_event.window->root_sheet, event);
		break;
	case EVENT_TYPE_WINDOW_UNFOCUSED:
		if(sheet_exists(event->event_union.window_unfocused_event.window->root_sheet))event->event_union.window_unfocused_event.window->root_sheet->event_procedure(event->event_union.window_unfocused_event.window->root_sheet, event);
		break;
	}
	allow_switch_task();
}

void fill_box_sheet(Sheet *sheet, short x, short y, unsigned short width, unsigned short height, Color color)
{
	for(short y_i = y; y_i < y + (short)height; y_i++)
	{
		if(y_i < 0)
		{
			y_i = -1;
			continue;
		}
		if(sheet->height <= y_i)break;
		for(short x_i = x; x_i < x + (short)width; x_i++)
		{
			if(x_i < 0)
			{
				x_i = -1;
				continue;
			}
			if(sheet->width <= x_i)break;
			put_dot_sheet(sheet, (unsigned short)x_i, (unsigned short)y_i, color);
		}
	}
}

void focus_sheet(Sheet *sheet)
{
	Event sheet_focused_event;
	if(focused_sheet)
	{
		Event sheet_unfocused_event;
		sheet_unfocused_event.type = EVENT_TYPE_SHEET_UNFOCUSED;
		sheet_unfocused_event.event_union.sheet_unfocused_event.sheet = focused_sheet;
		enqueue(sheet_unfocused_event.event_union.sheet_unfocused_event.sheet->event_queue, &sheet_unfocused_event);
	}
	focused_sheet = sheet;
	sheet_focused_event.type = EVENT_TYPE_SHEET_FOCUSED;
	sheet_focused_event.event_union.sheet_focused_event.sheet = focused_sheet;
	enqueue(sheet_focused_event.event_union.sheet_focused_event.sheet->event_queue, &sheet_focused_event);
}

short get_sheet_x_on_screen(Sheet const *sheet)
{
	short result;
	prohibit_switch_task();
	if(sheet->parent)
	{
		result = get_sheet_x_on_screen(sheet->parent) + sheet->x;
	}
	else
	{
		result = sheet->x;
	}
	allow_switch_task();
	return result;
}

short get_sheet_y_on_screen(Sheet const *sheet)
{
	short result;
	prohibit_switch_task();
	if(sheet->parent)
	{
		result = get_sheet_y_on_screen(sheet->parent) + sheet->y;
	}
	else
	{
		result = sheet->y;
	}
	allow_switch_task();
	return result;
}

Sheet *get_uppest_sheet(Sheet *sheet, unsigned short x, unsigned short y)
{
	prohibit_switch_task();
	for(Sheet *child = sheet->uppest_child; child; child = child->lower)
	{
		short x_on_child = (short)x -child->x;
		short y_on_child = (short)y -child->y;
		if(0 <= x_on_child && x_on_child < child->width && 0 <= y_on_child && y_on_child < child->height)
		{
			Sheet *result = get_uppest_sheet(child, x_on_child, y_on_child);
			allow_switch_task();
			return result;
		}
	}
	allow_switch_task();
	return sheet;
}

void init_sheets(Sheet **_background_sheet, void *(*background_sheet_procedure)(struct _Sheet *sheet, struct _Event const *event), Sheet **_mouse_cursor_sheet, Queue *event_queue)
{
	*_background_sheet = create_sheet(NULL, 0, 0, get_video_information()->width, get_video_information()->height, background_sheet_procedure, event_queue);
	focus_sheet(*_background_sheet);
	fill_box_sheet(background_sheet, 0, 0, background_sheet->width, background_sheet->height, color_black);
	*_mouse_cursor_sheet = create_sheet(NULL, get_video_information()->width / 2, get_video_information()->height / 2, 0x08, 0x10, NULL, event_queue);
	for(unsigned short mouse_cursor_image_y = 0; mouse_cursor_image_y < mouse_cursor_sheet->height; mouse_cursor_image_y++)for(unsigned short mouse_cursor_image_x = 0; mouse_cursor_image_x < mouse_cursor_sheet->width; mouse_cursor_image_x++)put_dot_sheet(mouse_cursor_sheet, mouse_cursor_image_x, mouse_cursor_image_y, mouse_cursor_image[mouse_cursor_image_x + mouse_cursor_image_y * mouse_cursor_sheet->width]);
}

bool is_descendant_sheet_of(Sheet const *descendant_candidate, Sheet const *ancestor_candidate)
{
	prohibit_switch_task();
	for(Sheet const *ancestor_candidate_child = ancestor_candidate->lowest_child; ancestor_candidate_child; ancestor_candidate_child = ancestor_candidate_child->upper)
	{
		if(descendant_candidate == ancestor_candidate_child || is_descendant_sheet_of(descendant_candidate, ancestor_candidate_child))
		{
			allow_switch_task();
			return true;
		}
	}
	allow_switch_task();
	return false;
}

bool is_focused_sheet(Sheet const *sheet)
{
	return sheet == focused_sheet;
}

void move_sheet(Sheet *sheet, short x, short y)
{
	prohibit_switch_task();
	// Previous sheet boundary seen from previous sheet
	short previous_top = 0;
	short previous_bottom = sheet->height;
	short previous_left = 0;
	short previous_right = sheet->width;
	// New sheet boundary seen from previous sheet
	short new_top = previous_top + y - sheet->y;
	short new_bottom = previous_bottom + y - sheet->y;
	short new_left = previous_left + x - sheet->x;
	short new_right = previous_right + x - sheet->x;
	// Regions that should be erased in previous sheet.
	short erased_top_height = new_top - previous_top;
	short erased_bottom_height = previous_bottom - new_bottom;
	short erased_left_width = new_left - previous_left;
	short erased_right_width = previous_right - new_right;
	if(erased_top_height < 0)erased_top_height = 0;
	else if(sheet->height < erased_top_height)erased_top_height = sheet->height;
	if(erased_bottom_height < 0)erased_bottom_height = 0;
	else if(sheet->height < erased_bottom_height)erased_bottom_height = sheet->height;
	if(erased_left_width < 0)erased_left_width = 0;
	else if(sheet->width < erased_left_width)erased_left_width = sheet->width;
	if(erased_right_width < 0)erased_right_width = 0;
	else if(sheet->width < erased_right_width)erased_right_width = sheet->width;
	// Erase self image at the old coordinate
	if(0 < erased_top_height)transmit_self_input_rectangle(sheet, 0, 0, sheet->width, erased_top_height);
	else if(0 < erased_bottom_height)transmit_self_input_rectangle(sheet, 0, sheet->height - erased_bottom_height, sheet->width, erased_bottom_height);
	if(0 < erased_left_width)transmit_self_input_rectangle(sheet, 0, erased_top_height, erased_left_width, sheet->height - erased_top_height - erased_bottom_height);
	else if(0 < erased_right_width)transmit_self_input_rectangle(sheet, sheet->width - erased_right_width, erased_top_height, erased_right_width, sheet->height - erased_top_height - erased_bottom_height);
	// Set the new coordinate
	sheet->x = x;
	sheet->y = y;
	// Refresh screen 
	refresh_input(sheet);
	refresh_self_output(sheet);
	transmit_self_output_through_opaques(sheet);
	allow_switch_task();
}

void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char const *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(((unsigned int)&format - (unsigned int)&sheet) / sizeof(unsigned int));
	char *output_string = create_char_array_from_chain_string(output_chain_string);
	print_sheet(sheet, x, y, foreground, background, output_string);
	free(output_string);
	delete_chain_string(output_chain_string);
}

void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char const *string)
{
	unsigned int char_pos_x = 0;
	unsigned int char_pos_y = 0;
	unsigned int char_pos_x_destroyable;
	prohibit_switch_task();
	while(*string)
	{
		switch(*string)
		{
		case '\t':
			char_pos_x_destroyable = (char_pos_x / TAB_LENGTH + 1) * TAB_LENGTH;
			fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH * (char_pos_x_destroyable - char_pos_x), CHAR_HEIGHT, background);
			char_pos_x = char_pos_x_destroyable;
			break;
		case '\n':
			char_pos_x = 0;
			char_pos_y++;
			break;
		case ' ':
			fill_box_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, CHAR_WIDTH, CHAR_HEIGHT, background);
			char_pos_x++;
			break;
		default:
			put_char_sheet(sheet, x + CHAR_WIDTH * char_pos_x, y + CHAR_HEIGHT * char_pos_y, foreground, background, *string);
			char_pos_x++;
			break;
		}
		string++;
	}
	allow_switch_task();
}

void print_sheet_tree(void)
{
	prohibit_switch_task();
	printf_serial("sheet tree\n");
	print_sheet_tree_level(background_sheet, 0);
	allow_switch_task();
}

void print_sheet_tree_level(Sheet *sheet, unsigned int level)
{
	prohibit_switch_task();
	for(unsigned int i = 0; i < level; i++)printf_serial("\t");
	printf_serial("sheet = %p\n", sheet);
	for(unsigned int i = 0; i < level; i++)printf_serial("\t");
	printf_serial("\tparent = %p\n", sheet->parent);
	for(unsigned int i = 0; i < level; i++)printf_serial("\t");
	printf_serial("\tupper = %p\n", sheet->upper);
	for(unsigned int i = 0; i < level; i++)printf_serial("\t");
	printf_serial("\tlower = %p\n", sheet->lower);
	for(unsigned int i = 0; i < level; i++)printf_serial("\t");
	printf_serial("\tuppest_child = %p\n", sheet->uppest_child);
	for(unsigned int i = 0; i < level; i++)printf_serial("\t");
	printf_serial("\tlowest_child = %p\n", sheet->lowest_child);
	if(sheet->lowest_child)print_sheet_tree_level(sheet->lowest_child, level + 1);
	if(sheet->upper)print_sheet_tree_level(sheet->upper, level);
	allow_switch_task();
}

void pull_up_sheet(Sheet *sheet)
{
	prohibit_switch_task();
	if(sheet->parent)
	{
		if(sheet->parent->uppest_child == sheet)
		{
			allow_switch_task();
			return; // The sheet is already the uppest
		}
		// erase the sheet before pulled up
		transmit_self_input(sheet);
		// Pull up the sheet
		prohibit_switch_task();
		if(sheet->parent->lowest_child == sheet)sheet->parent->lowest_child = sheet->upper;
		if(sheet->upper)sheet->upper->lower = sheet->lower;
		if(sheet->lower)sheet->lower->upper = sheet->upper;
		sheet->upper = NULL;
		sheet->lower = sheet->parent->uppest_child;
		sheet->parent->uppest_child->upper = sheet;
		sheet->parent->uppest_child = sheet;
		allow_switch_task();
		refresh_input(sheet);
		refresh_self_output(sheet);
		transmit_self_output_through_opaques(sheet);
	}
	else ERROR(); // Sheet that has no parent can't be pulled up.
	allow_switch_task();
}

void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character)
{
	prohibit_switch_task();
	switch(character)
	{
	case '\t':
	case '\n':
	case ' ':
		fill_box_sheet(sheet, x, y, CHAR_WIDTH, CHAR_HEIGHT, background);
		break;
	default:
		for(short y_i = 0; y_i < CHAR_HEIGHT; y_i++)
		{
			if(y + y_i < 0)
			{
				y_i = -y - 1;
				continue;
			}
			if(sheet->height <= y + y_i)break;
			for(short x_i = 0; x_i < CHAR_WIDTH; x_i++)
			{
				if(x + x_i < 0)
				{
					x_i = -x - 1;
					continue;
				}
				if(sheet->width <= x + x_i)break;
				if(get_font_pixel(character, x_i, y_i))put_dot_sheet(sheet, (unsigned short)(x + x_i), (unsigned short)(y + y_i), foreground);
				else put_dot_sheet(sheet, (unsigned short)(x + x_i), (unsigned short)(y + y_i), background);
			}
		}
		break;
	}
	allow_switch_task();
}

void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color)
{
	prohibit_switch_task();
	if(x < sheet->width && y < sheet->height)
	{
		sheet->image[x + y * sheet->width] = color;
		if(color.alpha)
		{
			sheet->self_output[x + y * sheet->width] = alpha_blend(color, sheet->input[x + y * sheet->width]);
			transmit_self_output_dot(sheet, x, y);
		}
	}
	else ERROR();
	allow_switch_task();
}

void refresh_input(Sheet *sheet) // refresh sheet->input.
{
	prohibit_switch_task();
	for(unsigned short x = 0; x < sheet->width; x++)for(unsigned short y = 0; y < sheet->height; y++)refresh_input_dot(sheet, x, y);
	allow_switch_task();
}

void refresh_input_dot(Sheet *sheet, unsigned short x, unsigned short y) // refresh sheet->input[x + y * sheet->width].
{
	prohibit_switch_task();
	if(x < sheet->width && y < sheet->height)
	{
		if(sheet->parent || sheet->lower)
		{
			unsigned short parent_width = (sheet->parent ? sheet->parent->width : get_video_information()->width);
			unsigned short parent_height = (sheet->parent ? sheet->parent->height : get_video_information()->height);
			short x_on_parent = x + sheet->x;
			short y_on_parent = y + sheet->y;
			if(0 <= x_on_parent && x_on_parent < parent_width && 0 <= y_on_parent && y_on_parent < parent_height)
			{
				for(Sheet *lower = sheet->lower; lower; lower = lower->lower)
				{
					short x_on_lower = x_on_parent - lower->x;
					short y_on_lower = y_on_parent - lower->y;
					if(0 <= x_on_lower && x_on_lower < lower->width && 0 <= y_on_lower && y_on_lower < lower->height)
					{
						sheet->input[x + y * sheet->width] = lower->family_output[x_on_lower + y_on_lower * lower->width];
						allow_switch_task();
						return;
					}
				}
				sheet->input[x + y * sheet->width] = (sheet->parent ? sheet->parent->self_output[x_on_parent + y_on_parent * sheet->parent->width] : color_black);
			}
		}
		else sheet->input[x + y * sheet->width] = color_black;
	}
	else ERROR();
	allow_switch_task();
}

void refresh_self_output(Sheet *sheet) // refresh sheet->self_output.
{
	prohibit_switch_task();
	for(unsigned short x = 0; x < sheet->width; x++)for(unsigned short y = 0; y < sheet->height; y++)refresh_self_output_dot(sheet, x, y);
	allow_switch_task();
}

void refresh_self_output_dot(Sheet *sheet, unsigned short x, unsigned short y) // refresh sheet->self_output[x + y * sheet->width].
{
	prohibit_switch_task();
	refresh_input_dot(sheet, x, y);
	sheet->self_output[x + y * sheet->width] = alpha_blend(sheet->image[x + y * sheet->width], sheet->input[x + y * sheet->width]);
	allow_switch_task();
}

void send_sheets_event(Event const *event)
{
	Event new_event;
	static Sheet *left_button_catched_sheet = NULL;
	static Sheet *middle_button_catched_sheet = NULL;
	static Sheet *right_button_catched_sheet = NULL;
	static Sheet *fourth_button_catched_sheet = NULL;
	static Sheet *fifth_button_catched_sheet = NULL;
	prohibit_switch_task();
	switch(event->type)
	{
	case EVENT_TYPE_KEYBOARD_EVENT:
		if(focused_sheet)
		{
			new_event.type = EVENT_TYPE_SHEET_KEYBOARD;
			new_event.event_union.sheet_keyboard_event.keyboard_event = event->event_union.keyboard_event;
			new_event.event_union.sheet_keyboard_event.sheet = focused_sheet;
			enqueue(new_event.event_union.sheet_keyboard_event.sheet->event_queue, &new_event);
		}
		break;
	case EVENT_TYPE_MOUSE_EVENT:
		if(event->event_union.mouse_event.flags & (MOUSE_LEFT_BUTTON_PUSHED_NOW | MOUSE_MIDDLE_BUTTON_PUSHED_NOW | MOUSE_RIGHT_BUTTON_PUSHED_NOW | MOUSE_4TH_BUTTON_PUSHED_NOW | MOUSE_5TH_BUTTON_PUSHED_NOW))
		{
			// Mouse button is pushed now.
			Sheet *clicked_sheet = get_uppest_sheet(background_sheet, event->event_union.mouse_event.x, event->event_union.mouse_event.y);
			new_event.type = EVENT_TYPE_SHEET_CLICKED;
			new_event.event_union.sheet_clicked_event.sheet = clicked_sheet;
			new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(clicked_sheet);
			new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(clicked_sheet);
			if(event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED_NOW)
			{
				left_button_catched_sheet = clicked_sheet;
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED_NOW)
			{
				middle_button_catched_sheet = clicked_sheet;
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_MIDDLE_BUTTON;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED_NOW)
			{
				right_button_catched_sheet = clicked_sheet;
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_RIGHT_BUTTON;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED_NOW)
			{
				fourth_button_catched_sheet = clicked_sheet;
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_4TH_BUTTON;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED_NOW)
			{
				fifth_button_catched_sheet = clicked_sheet;
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_5TH_BUTTON;
			}
			enqueue(clicked_sheet->event_queue, &new_event);
		}
		if(event->event_union.mouse_event.flags & (MOUSE_LEFT_BUTTON_RELEASED_NOW | MOUSE_MIDDLE_BUTTON_RELEASED_NOW | MOUSE_RIGHT_BUTTON_RELEASED_NOW | MOUSE_4TH_BUTTON_RELEASED_NOW | MOUSE_5TH_BUTTON_RELEASED_NOW))
		{
			// Mouse button is released now.
			new_event.type = EVENT_TYPE_SHEET_CLICKED;
			if(event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_RELEASED_NOW && sheet_exists(left_button_catched_sheet))
			{
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON;
				new_event.event_union.sheet_clicked_event.sheet = left_button_catched_sheet;
				new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(left_button_catched_sheet);
				new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(left_button_catched_sheet);
				left_button_catched_sheet = NULL;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_RELEASED_NOW && sheet_exists(middle_button_catched_sheet))
			{
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_MIDDLE_BUTTON;
				new_event.event_union.sheet_clicked_event.sheet = middle_button_catched_sheet;
				new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(middle_button_catched_sheet);
				new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(middle_button_catched_sheet);
				middle_button_catched_sheet = NULL;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_RELEASED_NOW && sheet_exists(right_button_catched_sheet))
			{
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_RIGHT_BUTTON;
				new_event.event_union.sheet_clicked_event.sheet = right_button_catched_sheet;
				new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(right_button_catched_sheet);
				new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(right_button_catched_sheet);
				right_button_catched_sheet = NULL;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_RELEASED_NOW && sheet_exists(fourth_button_catched_sheet))
			{
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_4TH_BUTTON;
				new_event.event_union.sheet_clicked_event.sheet = fourth_button_catched_sheet;
				new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(fourth_button_catched_sheet);
				new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(fourth_button_catched_sheet);
				fourth_button_catched_sheet = NULL;
			}
			else if(event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_RELEASED_NOW && sheet_exists(fifth_button_catched_sheet))
			{
				new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_5TH_BUTTON;
				new_event.event_union.sheet_clicked_event.sheet = fifth_button_catched_sheet;
				new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(fifth_button_catched_sheet);
				new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(fifth_button_catched_sheet);
				fifth_button_catched_sheet = NULL;
			}
			enqueue(new_event.event_union.sheet_clicked_event.sheet->event_queue, &new_event);
		}
		if(event->event_union.mouse_event.x_movement || event->event_union.mouse_event.y_movement)
		{
			// Mouse is moving now.
			// Move mouse cursor.
			move_sheet(mouse_cursor_sheet, event->event_union.mouse_event.x, event->event_union.mouse_event.y);
			// Move left button catched window.
			if(sheet_exists(left_button_catched_sheet))
			{
				new_event.type = EVENT_TYPE_SHEET_MOUSE_DRAG;
				new_event.event_union.sheet_mouse_drag_event.sheet = left_button_catched_sheet;
				new_event.event_union.sheet_mouse_drag_event.x_movement = event->event_union.mouse_event.x_movement;
				new_event.event_union.sheet_mouse_drag_event.y_movement = event->event_union.mouse_event.y_movement;
				enqueue(left_button_catched_sheet->event_queue, &new_event);
			}
			else
			{
				Sheet *sheet_under_cursor = get_uppest_sheet(background_sheet, event->event_union.mouse_event.x, event->event_union.mouse_event.y);
				new_event.type = EVENT_TYPE_SHEET_MOUSE_MOVE;
				new_event.event_union.sheet_mouse_move_event.sheet = sheet_under_cursor;
				new_event.event_union.sheet_mouse_move_event.x_movement = event->event_union.mouse_event.x_movement;
				new_event.event_union.sheet_mouse_move_event.y_movement = event->event_union.mouse_event.y_movement;
				enqueue(new_event.event_union.sheet_mouse_move_event.sheet->event_queue, &new_event);
			}
		}
		if(event->event_union.mouse_event.vertical_wheel_movement)
		{
			Sheet *sheet_under_cursor = get_uppest_sheet(background_sheet, event->event_union.mouse_event.x, event->event_union.mouse_event.y);
			new_event.type = EVENT_TYPE_SHEET_VERTICAL_WHEEL;
			new_event.event_union.sheet_vertical_wheel_event.sheet = sheet_under_cursor;
			new_event.event_union.sheet_vertical_wheel_event.rotation = event->event_union.mouse_event.vertical_wheel_movement;
			new_event.event_union.sheet_vertical_wheel_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(sheet_under_cursor);
			new_event.event_union.sheet_vertical_wheel_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(sheet_under_cursor);
			enqueue(new_event.event_union.sheet_vertical_wheel_event.sheet->event_queue, &new_event);
		}
		break;
	}
	allow_switch_task();
}

void set_default_procedure(Sheet *sheet)
{
	prohibit_switch_task();
	for(Sheet *child = sheet->lowest_child; child; child = child->upper)set_default_procedure(child);
	sheet->event_procedure = default_event_procedure;
	allow_switch_task();
}

bool sheet_exists(Sheet const *sheet)
{
	bool result;
	prohibit_switch_task();
	result = sheet_exists_under(sheet, background_sheet);
	allow_switch_task();
	return result;
}

bool sheet_exists_under(Sheet const *sheet, Sheet const *root)
{
	prohibit_switch_task();
	if(root == sheet)
	{
		allow_switch_task();
		return true;
	}
	for(Sheet *child = root->lowest_child; child; child = child->upper)if(sheet_exists_under(sheet, child))
	{
		allow_switch_task();
		return true;
	}
	for(Sheet *upper = root->upper; upper; upper = upper->upper)if(sheet_exists_under(sheet, upper))
	{
		allow_switch_task();
		return true;
	}
	allow_switch_task();
	return false;
}

void transmit_family_output_dot(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->family_output[x + y * sheet->width].
{
	short x_on_screen;
	short y_on_screen;
	prohibit_switch_task();
	if(sheet->upper)
	{
		for(Sheet *upper = sheet->upper; upper; upper = upper->upper)
		{
			short x_on_upper = (short)x + sheet->x - upper->x;
			short y_on_upper = (short)y + sheet->y - upper->y;
			if(0 <= x_on_upper && x_on_upper < upper->width && 0 <= y_on_upper && y_on_upper < upper->height)
			{
				upper->input[x_on_upper + y_on_upper * upper->width] = sheet->family_output[x + y * sheet->width];
				if(upper->image[x_on_upper + y_on_upper * upper->width].alpha != 0xff)
				{
					upper->self_output[x_on_upper + y_on_upper * upper->width] = alpha_blend(upper->image[x_on_upper + y_on_upper * upper->width], upper->input[x_on_upper + y_on_upper * upper->width]);
					transmit_self_output_dot(upper, x_on_upper, y_on_upper);
				}
				allow_switch_task();
				return;
			}
		}
	}
	if(sheet->parent)
	{
		short x_on_parent = x + sheet->x;
		short y_on_parent = y + sheet->y;
		if(0 <= x_on_parent && x_on_parent < sheet->parent->width && 0 <= y_on_parent && y_on_parent < sheet->parent->height)
		{
			sheet->parent->family_output[x_on_parent + y_on_parent * sheet->parent->width] = sheet->family_output[x + y * sheet->width];
			transmit_family_output_dot(sheet->parent, x_on_parent, y_on_parent);
			allow_switch_task();
			return;
		}
	}
	x_on_screen = x + sheet->x;
	y_on_screen = y + sheet->y;
	if(0 <= x_on_screen && x_on_screen < get_video_information()->width && 0 <= y_on_screen && y_on_screen < get_video_information()->height)put_dot_screen(x_on_screen, y_on_screen, sheet->family_output[x + y * sheet->width]);
	allow_switch_task();
}

void transmit_family_output_dot_through_opaques(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->family_output[x + y * sheet->width] through opaque sheets.
{
	short x_on_screen;
	short y_on_screen;
	prohibit_switch_task();
	if(sheet->upper)
	{
		for(Sheet *upper = sheet->upper; upper; upper = upper->upper)
		{
			short x_on_upper = (short)x + sheet->x - upper->x;
			short y_on_upper = (short)y + sheet->y - upper->y;
			if(0 <= x_on_upper && x_on_upper < upper->width && 0 <= y_on_upper && y_on_upper < upper->height)
			{
				upper->input[x_on_upper + y_on_upper * upper->width] = sheet->family_output[x + y * sheet->width];
				if(upper->image[x_on_upper + y_on_upper * upper->width].alpha != 0xff)upper->self_output[x_on_upper + y_on_upper * upper->width] = alpha_blend(upper->image[x_on_upper + y_on_upper * upper->width], upper->input[x_on_upper + y_on_upper * upper->width]);
				transmit_self_output_dot_through_opaques(upper, x_on_upper, y_on_upper);
				allow_switch_task();
				return;
			}
		}
	}
	if(sheet->parent)
	{
		short x_on_parent = x + sheet->x;
		short y_on_parent = y + sheet->y;
		if(0 <= x_on_parent && x_on_parent < sheet->parent->width && 0 <= y_on_parent && y_on_parent < sheet->parent->height)
		{
			sheet->parent->family_output[x_on_parent + y_on_parent * sheet->parent->width] = sheet->family_output[x + y * sheet->width];
			transmit_family_output_dot(sheet->parent, x_on_parent, y_on_parent);
			allow_switch_task();
			return;
		}
	}
	x_on_screen = x + sheet->x;
	y_on_screen = y + sheet->y;
	if(0 <= x_on_screen && x_on_screen < get_video_information()->width && 0 <= y_on_screen && y_on_screen < get_video_information()->height)put_dot_screen(x_on_screen, y_on_screen, sheet->family_output[x + y * sheet->width]);
	allow_switch_task();
}

void transmit_self_input(Sheet *sheet) // transmit image sheet->self_input.
{
	prohibit_switch_task();
	for(unsigned short y = 0; y < sheet->height; y++)for(unsigned short x = 0; x < sheet->width; x++)transmit_self_input_dot(sheet, x, y);
	allow_switch_task();
}

void transmit_self_input_dot(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->self_input[x + y * sheet->width].
{
	Color family_output;
	prohibit_switch_task();
	family_output = sheet->family_output[x + y * sheet->width];
	sheet->family_output[x + y * sheet->width] = sheet->input[x + y * sheet->width];
	transmit_family_output_dot(sheet, x, y);
	sheet->family_output[x + y * sheet->width] = family_output;
	allow_switch_task();
}

void transmit_self_input_rectangle(Sheet *sheet, unsigned short x, unsigned short y, unsigned short width, unsigned short height) // transmit color sheet->self_input[x + y * sheet->width].
{
	prohibit_switch_task();
	for(unsigned short y_i = y; y_i < y + height; y_i++)for(unsigned short x_i = x; x_i < x + width; x_i++)transmit_self_input_dot(sheet, x_i, y_i);
	allow_switch_task();
}

void transmit_self_output(Sheet *sheet) // transmit image sheet->self_output.
{
	prohibit_switch_task();
	for(unsigned short y = 0; y < sheet->height; y++)for(unsigned short x = 0; x < sheet->width; x++)transmit_self_output_dot(sheet, x, y);
	allow_switch_task();
}

void transmit_self_output_dot(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->self_output[x + y * sheet->width].
{
	prohibit_switch_task();
	for(Sheet *child = sheet->lowest_child; child; child = child->upper)
	{
		short x_on_child = x - child->x;
		short y_on_child = y - child->y;
		if(0 <= x_on_child && x_on_child < child->width && 0 <= y_on_child && y_on_child < child->height)
		{
			child->input[x_on_child + y_on_child * child->width] = sheet->self_output[x + y * sheet->width];
			if(child->image[x_on_child + y_on_child * child->width].alpha != 0xff)
			{
				child->self_output[x_on_child + y_on_child * child->width] = alpha_blend(child->image[x_on_child + y_on_child * child->width], child->input[x_on_child + y_on_child * child->width]);
				transmit_self_output_dot(child, x_on_child, y_on_child);
			}
			allow_switch_task();
			return;
		}
	}
	sheet->family_output[x + y * sheet->width] = sheet->self_output[x + y * sheet->width];
	transmit_family_output_dot(sheet, x, y);
	allow_switch_task();
}

void transmit_self_output_dot_through_opaques(Sheet *sheet, unsigned short x, unsigned short y) // transmit color sheet->self_output[x + y * sheet->width] through opaque sheets.
{
	prohibit_switch_task();
	for(Sheet *child = sheet->lowest_child; child; child = child->upper)
	{
		short x_on_child = x - child->x;
		short y_on_child = y - child->y;
		if(0 <= x_on_child && x_on_child < child->width && 0 <= y_on_child && y_on_child < child->height)
		{
			child->input[x_on_child + y_on_child * child->width] = sheet->self_output[x + y * sheet->width];
			if(child->image[x_on_child + y_on_child * child->width].alpha != 0xff)child->self_output[x_on_child + y_on_child * child->width] = alpha_blend(child->image[x_on_child + y_on_child * child->width], child->input[x_on_child + y_on_child * child->width]);
			transmit_self_output_dot_through_opaques(child, x_on_child, y_on_child);
			allow_switch_task();
			return;
		}
	}
	sheet->family_output[x + y * sheet->width] = sheet->self_output[x + y * sheet->width];
	transmit_family_output_dot(sheet, x, y);
	allow_switch_task();
}

void transmit_self_output_rectangle(Sheet *sheet, unsigned short x, unsigned short y, unsigned short width, unsigned short height) // transmit color sheet->self_output[x + y * sheet->width].
{
	prohibit_switch_task();
	for(unsigned short y_i = y; y_i < y + height; y_i++)for(unsigned short x_i = x; x_i < x + width; x_i++)transmit_self_output_dot(sheet, x_i, y_i);
	allow_switch_task();
}

void transmit_self_output_through_opaques(Sheet *sheet) // transmit image sheet->self_output through opaque sheets.
{
	prohibit_switch_task();
	for(unsigned short y = 0; y < sheet->height; y++)for(unsigned short x = 0; x < sheet->width; x++)transmit_self_output_dot_through_opaques(sheet, x, y);
	allow_switch_task();
}

