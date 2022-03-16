#include "console.h"
#include "memory.h"
#include "serial.h"

Console *root_console = NULL;

void *console_event_procedure(Sheet *sheet, struct _Event const *event);
void delete_console(Console *console);
Console *get_console_from_sheet(Sheet const *sheet);

void *console_event_procedure(Sheet *sheet, struct _Event const *event)
{
	Console *console = get_console_from_sheet(sheet);
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		delete_console(console);
		return console->default_event_procedure(sheet, event);
	default:
		return console->default_event_procedure(sheet, event);
	}
}

void delete_console(Console *console)
{
	printf_serial("Delete console %p\n", console);
	prohibit_switch_task();
	console->text_box->sheet->event_procedure = console->default_event_procedure;
	if(root_console == console)root_console = root_console->next;
	if(root_console == console)root_console = NULL;
	console->previous->next = console->next;
	console->next->previous = console->previous;
	free(console);
	allow_switch_task();
}

Console *get_console_from_sheet(Sheet const *sheet)
{
	Console *candidate_console;
	prohibit_switch_task();
	candidate_console = root_console;
	do
	{
		if(candidate_console->text_box->sheet == sheet)
		{
			allow_switch_task();
			return candidate_console;
		}
		candidate_console = candidate_console->next;
	} while(candidate_console != root_console);
	ERROR(); // Console is not found.
	allow_switch_task();
	return NULL;
}

Console *make_sheet_console(Sheet *sheet, Color foreground_color, Color background_color)
{
	Console *new_console = malloc(sizeof(*new_console));
	printf_serial("Make sheet %p console %p\n", sheet, new_console);
	prohibit_switch_task();
	new_console->text_box = make_sheet_text_box(sheet, foreground_color, background_color);
	new_console->default_event_procedure = new_console->text_box->sheet->event_procedure;
	new_console->text_box->sheet->event_procedure = console_event_procedure;
	if(root_console)
	{
		new_console->next = root_console;
		new_console->previous = root_console->previous;
		root_console->previous->next = new_console;
		root_console->previous = new_console;
	}
	else
	{
		new_console->next = new_console;
		new_console->previous = new_console;
		root_console = new_console;
	}
	allow_switch_task();
	return new_console;
}

