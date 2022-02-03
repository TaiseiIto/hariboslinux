#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "sheet.h"

typedef struct _Window
{
	Sheet *root_sheet;
} Window;

Window *create_window(Sheet *background_sheet, short x, short y, unsigned short width, unsigned short height);

#endif

