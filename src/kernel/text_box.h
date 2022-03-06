#ifndef _TEXT_BOX_H_
#define _TEXT_BOX_H_

#include "sheet.h"

typedef struct _TextBox
{
	Sheet *sheet;
	struct _TextBox *next;
	struct _TextBox *previous;
} TextBox;

TextBox *make_sheet_text_box(Sheet *sheet);

#endif

