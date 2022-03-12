#include "font.h"
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
	char character;
	unsigned int position_x;
	unsigned int position_y;
	if(text_box->cursor_position)
	{
		character = text_box->cursor_position->character->character;
		position_x = text_box->cursor_position->x;
		position_y = text_box->cursor_position->y;
	}
	else
	{
		character = ' ';
		position_x = text_box->last_position->x;
		position_y = text_box->last_position->y;
		if(text_box->width <= ++position_x)
		{
			position_x -= text_box->width;
			position_y++;
		}
	}
	text_box->flags ^= TEXT_BOX_FLAG_CURSOR_BLINK_ON;
	blink_on = text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON && is_focused_sheet(text_box->sheet);
	put_char_sheet(text_box->sheet, CHAR_WIDTH * position_x, CHAR_HEIGHT * position_y, blink_on ? text_box->background_color : text_box->foreground_color, blink_on ? text_box->foreground_color : text_box->background_color, character);
	return NULL;
}

void delete_text_box(TextBox *text_box)
{
	printf_serial("Delete text box %p\n", text_box);
	prohibit_switch_task();
	text_box_delete_chars(text_box, text_box->first_position, text_box->string->length);
	delete_timer(text_box->cursor_blink_timer);
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
	new_text_box->string = create_chain_string("");
	new_text_box->cursor_position = NULL;
	new_text_box->first_position = NULL;
	new_text_box->last_position = NULL;
	new_text_box->sheet = sheet;
	new_text_box->default_event_procedure = new_text_box->sheet->event_procedure;
	new_text_box->sheet->event_procedure = text_box_event_procedure;
	new_text_box->cursor_blink_timer = create_timer(0, 100, get_current_task()->event_queue, (void *(*)(void *))cursor_blink, (void *)new_text_box, NULL);
	new_text_box->foreground_color = foreground_color;
	new_text_box->background_color = background_color;
	new_text_box->height = new_text_box->sheet->height / CHAR_HEIGHT;
	new_text_box->width = new_text_box->sheet->width / CHAR_WIDTH;
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
		if(event->event_union.keyboard_event.character && event->event_union.keyboard_event.flags & KEYBOARD_FLAG_KEY_PUSHED)
		{
			// Insert input character.
			text_box_insert_char_front(text_box, text_box->cursor_position, event->event_union.keyboard_event.character);
			// Move cursor.
			text_box->cursor_position = text_box->cursor_position->next;
		}
		return NULL;
	default:
		return text_box->default_event_procedure(sheet, event);
	}
}

void text_box_delete_char(TextBox *text_box, CharacterPosition *position)
{
	unsigned int x = position->x;
	unsigned int y = position->y;
	// Delete the character
	delete_char(text_box->string, position->character);
	if(position->previous)position->previous->next = position->next;
	else if(position == text_box->first_position)text_box->first_position = position->next;
	else ERROR();
	if(position->next)position->next->previous = position->previous;
	else if(position == text_box->last_position)text_box->last_position = position->previous;
	else ERROR();
	// Relocate characters.
	for(CharacterPosition *position_i = position->next; position_i; position_i = position_i->next)
	{
		position_i->x = x;
		position_i->y = y;
		put_char_sheet(text_box->sheet, CHAR_WIDTH * x, CHAR_HEIGHT * y, text_box->foreground_color, text_box->background_color, position_i->character->character);
		if(text_box->width <= ++x)
		{
			x -= text_box->width;
			y++;
		}
	}
	free(position);
}

void text_box_delete_chars(TextBox *text_box, CharacterPosition *position, unsigned int length)
{
	CharacterPosition *next_position;
	for(unsigned int i = 0; i < length; i++)
	{
		if(!position)ERROR(); // The position doesn't exist.
		next_position = position->next;
		text_box_delete_char(text_box, position);
		position = next_position;
	}
}

void text_box_insert_char_front(TextBox *text_box, CharacterPosition *position, char wedge)
{
	CharacterPosition *new_position;
	// Insert the character
	insert_char_front(text_box->string, position->character, wedge);
	// Prepare new position for the new character.
	new_position = malloc(sizeof(*new_position));
	if(position)
	{
		new_position->character = position->character->previous;
		new_position->x = position->x;
		new_position->y = position->y;
		new_position->next = position;
		new_position->previous = position->previous;
		if(position->previous)position->previous->next = new_position;
		else if(position == text_box->first_position)text_box->first_position = new_position;
		else ERROR();
		position->previous = new_position;
	}
	else
	{
		if(text_box->first_position && text_box->last_position)
		{
			new_position->character = text_box->string->last_character;
			new_position->x = text_box->last_position->x;
			new_position->y = text_box->last_position->y;
			if(text_box->width <= ++new_position->x)
			{
				new_position->x -= text_box->width;
				new_position->y++;
			}
			new_position->previous = text_box->last_position;
			new_position->next = NULL;
			text_box->last_position->next = new_position;
			text_box->last_position = new_position;
		}
		else if(!text_box->first_position && !text_box->last_position)
		{
			new_position->character = text_box->string->last_character;
			new_position->x = 0;
			new_position->y = 0;
			new_position->previous = NULL;
			new_position->next = NULL;
			text_box->first_position = new_position;
			text_box->last_position = new_position;
		}
		else ERROR(); // The position list is broken.
	}
	// Relocate characters.
	unsigned int x = new_position->x;
	unsigned int y = new_position->y;
	for(CharacterPosition *position_i = new_position; position_i; position_i = position_i->next)
	{
		position_i->x = x;
		position_i->y = y;
		put_char_sheet(text_box->sheet, CHAR_WIDTH * x, CHAR_HEIGHT * y, text_box->foreground_color, text_box->background_color, position_i->character->character);
		if(text_box->width <= ++x)
		{
			x -= text_box->width;
			y++;
		}
	}
}

