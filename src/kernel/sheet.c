#include "memory.h"
#include "serial.h"
#include "sheet.h"
#include "task.h"

Sheet *uppest_sheet = NULL;
Sheet *lowest_sheet = NULL;

Sheet *create_sheet(short x, short y, unsigned short width, unsigned short height)
{
	Sheet *new_sheet = malloc(sizeof(*new_sheet));
	new_sheet->x = x;
	new_sheet->y = y;
	new_sheet->width = width;
	new_sheet->height = height;
	new_sheet->image = malloc(width * height * sizeof(*new_sheet->image));
	cli_task();
	if(lowest_sheet)
	{
		if(uppest_sheet)
		{
			new_sheet->lower_sheet = uppest_sheet;
			new_sheet->upper_sheet = NULL;
			uppest_sheet->upper_sheet = new_sheet;
			uppest_sheet = new_sheet;
		}
		else ERROR_MESSAGE();
	}
	else
	{
		if(uppest_sheet)ERROR_MESSAGE();
		else
		{
			new_sheet->lower_sheet = NULL;
			new_sheet->upper_sheet = NULL;
			lowest_sheet = new_sheet;
			uppest_sheet = new_sheet;
		}
	}
	sti_task();
	return new_sheet;
}

void delete_sheet(Sheet *sheet)
{
	cli_task();
	if(lowest_sheet == sheet)lowest_sheet = sheet->upper_sheet;
	if(uppest_sheet == sheet)uppest_sheet = sheet->lower_sheet;
	if(sheet->lower_sheet)sheet->lower_sheet->upper_sheet = sheet->upper_sheet;
	if(sheet->upper_sheet)sheet->upper_sheet->lower_sheet = sheet->lower_sheet;
	sti_task();
	free(sheet->image);
	free(sheet);
}

void printf_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *format, ...)
{
}

void print_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char *string)
{
}

void put_char_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color foreground, Color background, char character)
{
}

void put_dot_sheet(Sheet *sheet, unsigned short x, unsigned short y, Color color)
{
}

