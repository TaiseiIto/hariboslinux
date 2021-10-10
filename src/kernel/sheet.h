#ifndef _SHEET_H_
#define _SHEET_H_

#include "graphic.h"

typedef struct _Sheet
{
	Color *image;
	short x, y;
	unsigned short width, height;
	struct _Sheet *upper_sheet;
	struct _Sheet *lower_sheet;
} Sheet;

Sheet *create_sheet(short x, short y, unsigned short width, unsigned short height);
void delete_sheet(Sheet *sheet);

#endif

