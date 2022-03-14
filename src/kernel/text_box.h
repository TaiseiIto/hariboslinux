#ifndef _TEXT_BOX_H_
#define _TEXT_BOX_H_

#include "chain_string.h"
#include "sheet.h"

typedef struct _CharacterPosition
{
	ChainCharacter *character;
	unsigned int x;
	unsigned int y;
	int past_x_on_sheet;
	int past_y_on_sheet;
	struct _CharacterPosition *previous;
	struct _CharacterPosition *next;
} CharacterPosition;

typedef struct _TextBox
{
	ChainString *string;
	CharacterPosition *cursor_position;
	CharacterPosition *first_position;
	CharacterPosition *last_position;
	Sheet *sheet;
	void *(*default_event_procedure)(struct _Sheet *sheet, struct _Event const *event);
	Timer *cursor_blink_timer;
	struct _TextBox *next;
	struct _TextBox *previous;
	Color foreground_color;
	Color background_color;
	unsigned int height; // sheet height / CHAR_HEIGHT
	unsigned int width; // sheet width / CHAR_WIDTH
	unsigned int scroll_amount;
	unsigned char flags;
	#define TEXT_BOX_FLAG_CURSOR_BLINK_ON 0x01
} TextBox;

TextBox *make_sheet_text_box(Sheet *sheet, Color foreground_color, Color background_color);
void text_box_delete_char(TextBox *text_box, CharacterPosition *position);
void text_box_delete_chars(TextBox *text_box, CharacterPosition *position, unsigned int length);
void text_box_insert_char_front(TextBox *text_box, CharacterPosition *position, char wedge);

#endif

