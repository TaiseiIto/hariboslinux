#ifndef _CONSOLE_H_
#define _CONSOLE_H_

struct _Console;

#include "shell.h"
#include "text_box.h"

typedef struct _Console
{
	CharacterPosition *prompt_position;
	TextBox *text_box;
	void *(*default_event_procedure)(struct _Sheet *sheet, struct _Event const *event);
	struct _Console *next;
	struct _Console *previous;
	struct _Shell *shell;
} Console;

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

typedef struct _ConsoleTaskArgument
{
	Sheet *background_sheet;
} ConsoleTaskArgument;

void *console_event_procedure(Sheet *sheet, struct _Event const *event);
Console *make_sheet_console(Sheet *sheet, Color foreground_color, Color background_color);
void console_task_procedure(ConsoleTaskArgument *console_task_argument);

#endif

