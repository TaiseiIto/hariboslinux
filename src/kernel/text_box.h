#ifndef _TEXT_BOX_H_
#define _TEXT_BOX_H_

#include "sheet.h"

typedef struct _TextBox
{
	Sheet *sheet;
	struct _TextBox *next;
	struct _TextBox *previous;
	void *(*default_event_procedure)(struct _Sheet *sheet, struct _Event const *event);
} TextBox;

TextBox *make_sheet_text_box(Sheet *sheet);

#endif

