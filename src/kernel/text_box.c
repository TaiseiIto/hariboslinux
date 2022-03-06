#include "memory.h"
#include "serial.h"
#include "text_box.h"

TextBox *root_text_box = NULL;

void delete_text_box(TextBox *text_box);
TextBox *get_text_box_from_sheet(Sheet *sheet);
void *text_box_event_procedure(Sheet *sheet, struct _Event const *event);

void delete_text_box(TextBox *text_box)
{
	printf_serial("Delete text box %p\n", text_box);
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
	new_text_box->sheet = sheet;
	new_text_box->default_event_procedure = new_text_box->sheet->event_procedure;
	new_text_box->sheet->event_procedure = text_box_event_procedure;
	new_text_box->foreground_color = foreground_color;
	new_text_box->background_color = background_color;
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
		fill_box_sheet(sheet, 0, 0, sheet->width, sheet->height, text_box->background_color);
		break;
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		delete_text_box(text_box);
		break;
	case EVENT_TYPE_SHEET_KEYBOARD:
		printf_serial("Keyboard event @ text box %p keycode = %#04x\n", text_box, event->event_union.sheet_keyboard_event.keyboard_event.keycode);
		break;
	}
	return text_box->default_event_procedure(sheet, event);
}

