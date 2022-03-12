#include "memory.h"
#include "serial.h"
#include "text_box.h"

TextBox *root_text_box = NULL;

void *cursor_blink(TextBox *text_box);
void delete_text_box(TextBox *text_box);
TextBox *get_text_box_from_sheet(Sheet *sheet);
void *text_box_event_procedure(Sheet *sheet, struct _Event const *event);

void *cursor_blink(TextBox *text_box)
{
	bool blink_on;
	text_box->flags ^= TEXT_BOX_FLAG_CURSOR_BLINK_ON;
	blink_on = text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON && is_focused_sheet(text_box->sheet);
	put_char_sheet(text_box->sheet, 0, 0, blink_on ? text_box->background_color : text_box->foreground_color, blink_on ? text_box->foreground_color : text_box->background_color, ' ');
	return NULL;
}

void delete_text_box(TextBox *text_box)
{
	printf_serial("Delete text box %p\n", text_box);
	delete_chain_string(text_box->chain_string);
	delete_timer(text_box->cursor_blink_timer);
	prohibit_switch_task();
	if(root_text_box == text_box)root_text_box = root_text_box->next;
	if(root_text_box == text_box)root_text_box = NULL;
	text_box->previous->next = text_box->next;
	text_box->next->previous = text_box->previous;
	free(text_box);
	allow_switch_task();
}

TextBox *get_text_box_from_sheet(Sheet *sheet)
{
	TextBox *candidate_text_box;
	prohibit_switch_task();
	candidate_text_box = root_text_box;
	do
	{
		if(candidate_text_box->sheet == sheet)
		{
			allow_switch_task();
			return candidate_text_box;
		}
		candidate_text_box = candidate_text_box->next;
	} while(candidate_text_box != root_text_box);
	ERROR(); // Text box is not found.
	allow_switch_task();
	return NULL;
}

TextBox *make_sheet_text_box(Sheet *sheet, Color foreground_color, Color background_color)
{
	TextBox *new_text_box = malloc(sizeof(*new_text_box));
	printf_serial("Make sheet %p text box %p\n", sheet, new_text_box);
	prohibit_switch_task();
	new_text_box->chain_string = create_chain_string("");
	new_text_box->sheet = sheet;
	new_text_box->default_event_procedure = new_text_box->sheet->event_procedure;
	new_text_box->sheet->event_procedure = text_box_event_procedure;
	new_text_box->cursor_blink_timer = create_timer(0, 100, get_current_task()->event_queue, (void *(*)(void *))cursor_blink, (void *)new_text_box, NULL);
	new_text_box->foreground_color = foreground_color;
	new_text_box->background_color = background_color;
	new_text_box->cursor_position_x = 0;
	new_text_box->cursor_position_y = 0;
	new_text_box->flags = 0;
	if(root_text_box)
	{
		new_text_box->next = root_text_box;
		new_text_box->previous = root_text_box->previous;
		root_text_box->previous->next = new_text_box;
		root_text_box->previous = new_text_box;
	}
	else
	{
		new_text_box->next = new_text_box;
		new_text_box->previous = new_text_box;
		root_text_box = new_text_box;
	}
	allow_switch_task();
	return new_text_box;
}

void *text_box_event_procedure(Sheet *sheet, struct _Event const *event)
{
	TextBox *text_box = get_text_box_from_sheet(sheet);
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		text_box->default_event_procedure(sheet, event);
		fill_box_sheet(sheet, 0, 0, sheet->width, sheet->height, text_box->background_color);
		return NULL;
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		delete_text_box(text_box);
		return text_box->default_event_procedure(sheet, event);
	case EVENT_TYPE_SHEET_KEYBOARD:
		if(event->event_union.keyboard_event.character)
		{
			char *string;
			insert_char(text_box->chain_string, text_box->chain_string->last_character, event->event_union.keyboard_event.character);
			string = create_char_array_from_chain_string(text_box->chain_string);
			printf_serial(string);
			free(string);
		}
		return NULL;
	default:
		return text_box->default_event_procedure(sheet, event);
	}
}

