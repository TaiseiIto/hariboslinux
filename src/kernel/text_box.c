#include "memory.h"
#include "serial.h"
#include "text_box.h"

TextBox *root_text_box = NULL;

TextBox *make_sheet_text_box(Sheet *sheet)
{
	TextBox *new_text_box = malloc(sizeof(*new_text_box));
	printf_serial("Make sheet %p text box %p\n", sheet, new_text_box);
	prohibit_switch_task();
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

