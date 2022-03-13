#include "font.h"
#include "memory.h"
#include "serial.h"
#include "text_box.h"

TextBox *root_text_box = NULL;

CharacterPosition get_cursor_position(TextBox const *text_box);
TextBox *get_text_box_from_sheet(Sheet *sheet);
void *cursor_blink(TextBox *text_box);
void delete_text_box(TextBox *text_box);
void refresh_text_box(TextBox *text_box);
void refresh_text_box_after_position(TextBox *text_box, CharacterPosition const *position);
void *text_box_event_procedure(Sheet *sheet, struct _Event const *event);

CharacterPosition get_cursor_position(TextBox const *text_box)
{
	if(text_box->cursor_position)return *text_box->cursor_position;
	else
	{
		CharacterPosition cursor_position;
		cursor_position.character = NULL;
		cursor_position.next = NULL;
		cursor_position.previous = text_box->last_position;
		if(cursor_position.previous)
		{
			cursor_position.x = cursor_position.previous->x;
			cursor_position.y = cursor_position.previous->y;
			switch(cursor_position.previous->character->character)
			{
			case '\n':
				cursor_position.x = 0;
				cursor_position.y++;
				break;
			case '\t':
				cursor_position.x += TAB_LENGTH;
				cursor_position.x /= TAB_LENGTH;
				cursor_position.x *= TAB_LENGTH;
				break;
			default:
				cursor_position.x++;
				break;
			}
			if(text_box->width <= cursor_position.x)
			{
				cursor_position.x -= text_box->width;
				cursor_position.y++;
			}

		}
		else
		{
			cursor_position.x = 0;
			cursor_position.y = 0;
		}
		return cursor_position;
	}
}

void *cursor_blink(TextBox *text_box)
{
	bool blink_on;
	CharacterPosition cursor_position = get_cursor_position(text_box);
	text_box->flags ^= TEXT_BOX_FLAG_CURSOR_BLINK_ON;
	blink_on = text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON && is_focused_sheet(text_box->sheet);
	put_char_sheet(text_box->sheet, CHAR_WIDTH * cursor_position.x, CHAR_HEIGHT * cursor_position.y, blink_on ? text_box->background_color : text_box->foreground_color, blink_on ? text_box->foreground_color : text_box->background_color, cursor_position.character ? cursor_position.character->character : ' ');
	return NULL;
}

void delete_text_box(TextBox *text_box)
{
	printf_serial("Delete text box %p\n", text_box);
	prohibit_switch_task();
	text_box_delete_chars(text_box, text_box->first_position, text_box->string->length);
	delete_chain_string(text_box->string);
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

void refresh_text_box(TextBox *text_box)
{
	unsigned int x = 0;
	unsigned int y = 0;
	for(CharacterPosition *position = text_box->first_position; position; position = position->next)
	{
		x = position->x;
		y = position->y;
		put_char_sheet(text_box->sheet, CHAR_WIDTH * position->x, CHAR_HEIGHT * position->y, text_box->foreground_color, text_box->background_color, position->character->character);
		if(position->character->character == '\n')fill_box_sheet(text_box->sheet, CHAR_WIDTH * x, CHAR_HEIGHT * y, text_box->sheet->width - CHAR_WIDTH * x, CHAR_HEIGHT, text_box->background_color);
	}
	if(x < text_box->width - 1)fill_box_sheet(text_box->sheet, CHAR_WIDTH * (x + 1), CHAR_HEIGHT * y, CHAR_WIDTH * (text_box->width - (x + 1)), CHAR_HEIGHT, text_box->background_color);
	if(CHAR_HEIGHT * (y + 1) < text_box->sheet->height)fill_box_sheet(text_box->sheet, 0, CHAR_HEIGHT * (y + 1), CHAR_WIDTH * text_box->width, text_box->sheet->height - CHAR_HEIGHT * (y + 1), text_box->background_color);
}

void refresh_text_box_after_position(TextBox *text_box, CharacterPosition const *position)
{
	unsigned int x = 0;
	unsigned int y = 0;
	for(CharacterPosition const *position_i = position; position_i; position_i = position_i->next)
	{
		x = position_i->x;
		y = position_i->y;
		put_char_sheet(text_box->sheet, CHAR_WIDTH * position_i->x, CHAR_HEIGHT * position_i->y, position_i == text_box->cursor_position ? text_box->background_color : text_box->foreground_color, position_i == text_box->cursor_position ? text_box->foreground_color : text_box->background_color, position_i->character->character);
	}
	x = text_box->last_position->x;
	y = text_box->last_position->y;
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
			switch(event->event_union.keyboard_event.character)
			{
			case '\b':
				if(text_box->first_position && text_box->last_position)text_box_delete_char(text_box, text_box->cursor_position && text_box->cursor_position->previous ? text_box->cursor_position->previous : text_box->last_position);
				break;
			default:
				// Insert input character.
				text_box_insert_char_front(text_box, text_box->cursor_position, event->event_union.keyboard_event.character);
				break;
			}
		}
		return NULL;
	default:
		return text_box->default_event_procedure(sheet, event);
	}
}

void text_box_delete_char(TextBox *text_box, CharacterPosition *position)
{
	bool *is_erased_position;
	unsigned int x;
	unsigned int y;
	// Register erased positions.
	is_erased_position = malloc((text_box->height + 1) * text_box->width * sizeof(*is_erased_position));
	for(y = 0; y <= text_box->height; y++)for(x = 0; x < text_box->width; x++)is_erased_position[text_box->width * y + x] = false;
	for(CharacterPosition *position_i = position; position_i; position_i = position_i->next)if(text_box->width * position_i->y + position_i->x < (text_box->height + 1) * text_box->width)is_erased_position[text_box->width * position_i->y + position_i->x] = true;
	// Delete the character.
	delete_char(text_box->string, position->character);
	if(position->previous)position->previous->next = position->next;
	else if(position == text_box->first_position)text_box->first_position = position->next;
	else ERROR();
	if(position->next)position->next->previous = position->previous;
	else if(position == text_box->last_position)text_box->last_position = position->previous;
	else ERROR();
	// Relocate characters.
	x = position->x;
	y = position->y;
	for(CharacterPosition *position_i = position->next; position_i; position_i = position_i->next)
	{
		bool position_changed = ((position_i->x != x) || (position_i->y != y));
		position_i->x = x;
		position_i->y = y;
		if(position_changed)put_char_sheet(text_box->sheet, CHAR_WIDTH * position_i->x, CHAR_HEIGHT * position_i->y, position_i == text_box->cursor_position && text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON ? text_box->background_color : text_box->foreground_color, position_i == text_box->cursor_position && text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON ? text_box->foreground_color : text_box->background_color, position_i->character->character);
		if(text_box->width * position_i->y + position_i->x < (text_box->height + 1) * text_box->width)is_erased_position[text_box->width * position_i->y + position_i->x] = false;
		switch(position_i->character->character)
		{
		case '\n':
			x = 0;
			y++;
			break;
		case '\t':
			x += TAB_LENGTH;
			x /= TAB_LENGTH;
			x *= TAB_LENGTH;
			break;
		default:
			x++;
			break;
		}
		if(text_box->width <= x)
		{
			x -= text_box->width;
			y++;
		}
	}
	for(y = 0; y <= text_box->height; y++)for(x = 0; x < text_box->width; x++)if(is_erased_position[text_box->width * y + x])fill_box_sheet(text_box->sheet, CHAR_WIDTH * x, CHAR_HEIGHT * y, CHAR_WIDTH, CHAR_HEIGHT, text_box->background_color);
	free(is_erased_position);
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
	bool *is_erased_position;
	CharacterPosition *new_position;
	unsigned int x;
	unsigned int y;
	// Register erased positions.
	is_erased_position = malloc((text_box->height + 1) * text_box->width * sizeof(*is_erased_position));
	for(y = 0; y <= text_box->height; y++)for(x = 0; x < text_box->width; x++)is_erased_position[text_box->width * y + x] = false;
	for(CharacterPosition *position_i = position; position_i; position_i = position_i->next)if(text_box->width * position_i->y + position_i->x < (text_box->height + 1) * text_box->width)is_erased_position[text_box->width * position_i->y + position_i->x] = true;
	// Insert the character
	insert_char_front(text_box->string, position ? position->character : NULL, wedge);
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
			switch(text_box->last_position->character->character)
			{
			case '\n':
				new_position->x = 0;
				new_position->y++;
				break;
			case '\t':
				new_position->x += TAB_LENGTH;
				new_position->x /= TAB_LENGTH;
				new_position->x *= TAB_LENGTH;
				break;
			default:
				new_position->x++;
				break;
			}
			if(text_box->width <= new_position->x)
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
	x = new_position->x;
	y = new_position->y;
	for(CharacterPosition *position_i = new_position; position_i; position_i = position_i->next)
	{
		bool position_changed = ((position_i->x != x) || (position_i->y != y));
		position_i->x = x;
		position_i->y = y;
		if(position_changed || position_i == new_position)put_char_sheet(text_box->sheet, CHAR_WIDTH * position_i->x, CHAR_HEIGHT * position_i->y, position_i == text_box->cursor_position && text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON ? text_box->background_color : text_box->foreground_color, position_i == text_box->cursor_position && text_box->flags & TEXT_BOX_FLAG_CURSOR_BLINK_ON ? text_box->foreground_color : text_box->background_color, position_i->character->character);
		if(text_box->width * position_i->y + position_i->x < (text_box->height + 1) * text_box->width)is_erased_position[text_box->width * position_i->y + position_i->x] = false;
		switch(position_i->character->character)
		{
		case '\n':
			x = 0;
			y++;
			break;
		case '\t':
			x += TAB_LENGTH;
			x /= TAB_LENGTH;
			x *= TAB_LENGTH;
			break;
		default:
			x++;
			break;
		}
		if(text_box->width <= x)
		{
			x -= text_box->width;
			y++;
		}
	}
	for(y = 0; y <= text_box->height; y++)for(x = 0; x < text_box->width; x++)if(is_erased_position[text_box->width * y + x])fill_box_sheet(text_box->sheet, CHAR_WIDTH * x, CHAR_HEIGHT * y, CHAR_WIDTH, CHAR_HEIGHT, text_box->background_color);
	free(is_erased_position);
	// Check text_box->string.
	char *string = create_char_array_from_chain_string(text_box->string);
	printf_serial(string);
	free(string);
}

