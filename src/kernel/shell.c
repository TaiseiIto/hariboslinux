#include "memory.h"
#include "shell.h"

Shell *create_shell(Console *console)
{
	Shell *shell;
	shell = malloc(sizeof(*shell));
	if(console)
	{
		shell->console = console;
		shell->type = SHELL_TYPE_CONSOLE;
	}
	else
	{
		shell->console = NULL;
		shell->type = SHELL_TYPE_SERIAL;
	}
	return shell;
}

void delete_shell(Shell *shell)
{
	printf_serial("Delete shell %p\n", shell);
	free(shell);
}

void *execute_command(Shell *shell, char const * const command)
{
	printf_serial("%s shell %p executes command \"%s\"\n", shell->type == SHELL_TYPE_CONSOLE ? "Console" : "Serial" , shell, command);
	return NULL;
}

