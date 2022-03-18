#include "console.h"
#include "memory.h"
#include "serial.h"

typedef struct _CommandIssuedEvent
{
	char *command;
} CommandIssuedEvent;

typedef union _ConsoleEventUnion
{
	CommandIssuedEvent command_issued_event;
} ConsoleEventUnion;

typedef struct _ConsoleEvent
{
	unsigned char type;
	#define CONSOLE_EVENT_TYPE_PROMPT		0x00
	#define CONSOLE_EVENT_TYPE_COMMAND_ISSUED	0x01
	ConsoleEventUnion console_event_union;
} ConsoleEvent;

char const * const prompt = "> ";
Console *root_console = NULL;

void *console_event_procedure(Sheet *sheet, struct _Event const *event);
void delete_console(Console *console);
Console *get_console_from_sheet(Sheet const *sheet);

void *console_event_procedure(Sheet *sheet, struct _Event const *event)
{
	Console *console = get_console_from_sheet(sheet);
	ConsoleEvent *console_event;
	Event new_event;
	void *return_value;
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		return_value = console->default_event_procedure(sheet, event);
		// Send the first prompt event.
		console_event = malloc(sizeof(*console_event));
		console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
		new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
		new_event.event_union.sheet_user_defined_event.sheet = sheet;
		new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
		new_event.event_union.sheet_user_defined_event.any = console_event;
		enqueue(sheet->event_queue, &new_event);
		return return_value;
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		delete_console(console);
		return console->default_event_procedure(sheet, event);
	case EVENT_TYPE_SHEET_KEYBOARD:
		return_value = console->default_event_procedure(sheet, event);
		if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_KEY_PUSHED)switch(event->event_union.keyboard_event.character)
		{
		case '\n':
			if(!console->text_box->cursor_position)
			{
				// Send command issued event.
				console_event = malloc(sizeof(*console_event));
				console_event->type = CONSOLE_EVENT_TYPE_COMMAND_ISSUED;
				console_event->console_event_union.command_issued_event.command = create_char_array_from_chain_string(console->text_box->string);
				new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
				new_event.event_union.sheet_user_defined_event.sheet = sheet;
				new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
				new_event.event_union.sheet_user_defined_event.any = console_event;
				enqueue(sheet->event_queue, &new_event);
				// Send prompt event.
				console_event = malloc(sizeof(*console_event));
				console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
				new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
				new_event.event_union.sheet_user_defined_event.sheet = sheet;
				new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
				new_event.event_union.sheet_user_defined_event.any = console_event;
				enqueue(sheet->event_queue, &new_event);
			}
			break;
		}
		return return_value;
	case EVENT_TYPE_SHEET_USER_DEFINED:
		if(event->event_union.sheet_user_defined_event.procedure == console_event_procedure)
		{
			console_event = (ConsoleEvent *)event->event_union.sheet_user_defined_event.any;
			switch(console_event->type)
			{
			case CONSOLE_EVENT_TYPE_COMMAND_ISSUED:
				// Execute the command.
				printf_serial("Command \"%s\" issued.\n", console_event->console_event_union.command_issued_event.command);
				free(console_event->console_event_union.command_issued_event.command);
				break;
			case CONSOLE_EVENT_TYPE_PROMPT:
				// Print prompt.
				console->prompt_position = console->text_box->last_position;
				text_box_insert_string_back(console->text_box, console->text_box->last_position, prompt);
				console->prompt_position = console->prompt_position->next;
				break;
			default:
				ERROR();
				break;
			}
			free(console_event);
			return NULL;
		}
		else return console->default_event_procedure(sheet, event);
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
	new_console->prompt_position = NULL;
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

