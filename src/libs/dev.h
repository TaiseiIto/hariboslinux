#ifndef _DEV_H_
#define _DEV_H_

typedef struct _ConsoleCommand
{
	unsigned char type;
	#define CONSOLE_COMMAND_CLEAR	0x0
} ConsoleCommand;

void clear_console(void);

#endif

