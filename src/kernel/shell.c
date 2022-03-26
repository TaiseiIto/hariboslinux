#include "chain_string.h"
#include "io.h"
#include "memory.h"
#include "shell.h"
#include "stdio.h"

typedef struct _CommandLineArgument
{
	ChainString *chain_string;
	struct _CommandLineArgument *previous;
	struct _CommandLineArgument *next;
} CommandLineArgument;

char const * const prompt = "> ";

char **create_argv(char const *command);

char **create_argv(char const *command)
{
	CommandLineArgument *first_argument = malloc(sizeof(*first_argument));
	CommandLineArgument *last_argument = first_argument;
	CommandLineArgument *argument;
	CommandLineArgument *next_argument;
	unsigned int argc = 1;
	char **argv;
	unsigned char flags = 0;
	#define INSIDE_DOUBLE_QUOTATION 0x01
	first_argument->chain_string = create_chain_string("");
	first_argument->previous = NULL;
	first_argument->next = NULL;
	// Parse the command.
	for(; *command; command++)insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
	// Create argv.
	argv = malloc((argc + 1) * sizeof(*argv));
	argument = first_argument;
	for(unsigned int argv_index = 0; argv_index < argc; argv_index++)
	{
		argv[argv_index] = create_char_array_from_chain_string(argument->chain_string);
		argument = argument->next;
	}
	argv[argc] = NULL;
	// Discard chain strings.
	for(argument = first_argument; argument; argument = argument->next)delete_chain_string(argument->chain_string);
	// Discard arguments.
	for(argument = first_argument; argument; argument = next_argument)
	{
		next_argument = argument->next;
		free(argument);
	}
	return argv;
}

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
	unsigned int argc;
	char **argv;
	printf_shell(shell, "%s shell %p executes command \"%s\"\n", shell->type == SHELL_TYPE_CONSOLE ? "Console" : "Serial" , shell, command);
	// Create argv.
	argv = create_argv(command);
	// Count argc.
	for(argc = 0; argv[argc]; argc++);
	// Print argv.
	for(unsigned int argv_index = 0; argv_index < argc; argv_index++)printf_shell(shell, "argv[%d] = %s\n", argv_index, argv[argv_index]);
	// Discard argv.
	for(unsigned int argv_index = 0; argv_index < argc; argv_index++)free(argv[argv_index]);
	free(argv);
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

