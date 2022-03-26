#include "io.h"
#include "memory.h"
#include "shell.h"
#include "stdio.h"

char const * const prompt = "> ";

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

void *execute_command(Shell *shell, char const *command)
{
	printf_shell(shell, "%s shell %p executes command \"%s\"\n", shell->type == SHELL_TYPE_CONSOLE ? "Console" : "Serial" , shell, command);
	return NULL;
}

void print_shell(Shell *shell, char const *string)
{
	for(; *string; string++)put_char_shell(shell, *string);
}

void printf_shell(Shell *shell, char const *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(1);
	char *output_string = create_char_array_from_chain_string(output_chain_string);
	print_shell(shell, output_string);
	free(output_string);
	delete_chain_string(output_chain_string);
}

void put_char_shell(Shell *shell, char character)
{
	switch(shell->type)
	{
	case SHELL_TYPE_CONSOLE:
		text_box_insert_char_back(shell->console->text_box, shell->console->text_box->last_position, character);
		break;
	case SHELL_TYPE_SERIAL:
		put_char_serial(character);
		break;
	default:
		ERROR(); // Invalid shell type.
		break;
	}
}

