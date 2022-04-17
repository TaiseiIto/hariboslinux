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

typedef struct _ComTaskArgument
{
	char *com_file_name;
	void *com_file_binary;
	unsigned int com_file_size;
	unsigned int argc;
	char **argv;
} ComTaskArgument;

extern char const * const prompt;

Shell *create_shell(struct _Console *console);
void delete_shell(Shell *shell);
void *execute_command(Shell *shell, char const *command);
void print_shell(Shell *shell, char const *string);
void printf_shell(Shell *shell, char const *format, ...);
void put_char_shell(Shell *shell, char character);

#endif

