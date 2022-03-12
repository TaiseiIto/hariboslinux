#ifndef _TEXT_BOX_H_
#define _TEXT_BOX_H_

#include "chain_string.h"
#include "sheet.h"

typedef struct _TextBox
{
	ChainCharacter **characters; // characters[width * y + x] is a character at position (x, y) on the text box.
	ChainString *string;
	Sheet *sheet;
	void *(*default_event_procedure)(struct _Sheet *sheet, struct _Event const *event);
	Timer *cursor_blink_timer;
	struct _TextBox *next;
	struct _TextBox *previous;
	Color foreground_color;
	Color background_color;
	unsigned int cursor_position_x;
	unsigned int cursor_position_y;
	unsigned int height; // sheet height / CHAR_HEIGHT
	unsigned int width; // sheet width / CHAR_WIDTH
	unsigned char flags;
	#define TEXT_BOX_FLAG_CURSOR_BLINK_ON 0x01
} TextBox;

TextBox *make_sheet_text_box(Sheet *sheet, Color foreground_color, Color background_color);

#endif

