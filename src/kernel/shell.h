#ifndef _SHELL_H_
#define _SHELL_H_

struct _Shell;

#include "console.h"

typedef struct _Shell
{
	struct _Console *console;
	unsigned char type;
	#define SHELL_TYPE_CONSOLE	0x00
	#define SHELL_TYPE_SERIAL	0x01
} Shell;

Shell *create_shell(struct _Console *console);
void delete_shell(Shell *shell);
void *execute_command(Shell *shell, char const * const command);

#endif

