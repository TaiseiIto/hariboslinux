#include "chain_string.h"
#include "disk.h"
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
	#define INSIDE_QUOTATION 0x01
	#define INSIDE_DOUBLE_QUOTATION 0x02
	first_argument->chain_string = create_chain_string("");
	first_argument->previous = NULL;
	first_argument->next = NULL;
	// Parse the command.
	for(; *command; command++)switch(*command)
	{
	case ' ':
	case '\t':
		if(last_argument->chain_string->length)
		{
			if(!(flags & (INSIDE_QUOTATION | INSIDE_DOUBLE_QUOTATION)))
			{
				// Create a new argument.
				CommandLineArgument *new_argument = malloc(sizeof(*new_argument));
				new_argument->chain_string = create_chain_string("");
				new_argument->previous = last_argument;
				new_argument->next = NULL;
				last_argument->next = new_argument;
				last_argument = new_argument;
				argc++;
			}
			else insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		}
		break;
	case '\'':
		if(flags & INSIDE_QUOTATION)flags &= ~INSIDE_QUOTATION;
		else if(flags & INSIDE_DOUBLE_QUOTATION)insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		else
		{
			if(!last_argument->chain_string->length)flags |= INSIDE_QUOTATION;
			else insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		}
		break;
	case '\"':
		if(flags & INSIDE_QUOTATION)insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		else if(flags & INSIDE_DOUBLE_QUOTATION)flags &= ~INSIDE_DOUBLE_QUOTATION;
		else
		{
			if(!last_argument->chain_string->length)flags ^= INSIDE_DOUBLE_QUOTATION;
			else insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		}
		break;
	case '\\':
		if(*(command + 1))
		{
			command++;
			switch(*command)
			{
			case 'n':
				insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, '\n');
				break;
			case 't':
				insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, '\t');
				break;
			default:
				insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
				break;
			}
		}
		else insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		break;
	default:
		insert_char_back(last_argument->chain_string, last_argument->chain_string->last_character, *command);
		break;
	}
	if(!last_argument->chain_string->length)
	{
		if(last_argument->previous)
		{
			CommandLineArgument *empty_argument = last_argument;
			// Discard last empty argv.
			last_argument->previous->next = NULL;
			last_argument = last_argument->previous;
			delete_chain_string(empty_argument->chain_string);
			free(empty_argument);
			argc--;
		}
		else
		{
			// The command is empty.
			// Discard chain strings.
			for(argument = first_argument; argument; argument = argument->next)delete_chain_string(argument->chain_string);
			// Discard arguments.
			for(argument = first_argument; argument; argument = next_argument)
			{
				next_argument = argument->next;
				free(argument);
			}
			return NULL;
		}
	}
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
	if(argv)
	{
		char *executable_file_name;
		void *executable_file_binary;
		// Count argc.
		for(argc = 0; argv[argc]; argc++);
		// Print argv.
		for(unsigned int argv_index = 0; argv_index < argc; argv_index++)printf_shell(shell, "argv[%d] = \"%s\"\n", argv_index, argv[argv_index]);
		// Load a file specified by argv[0].
		executable_file_name = create_format_char_array("%s.elf", argv[0]);
		executable_file_binary = load_file(executable_file_name);
		if(executable_file_binary)
		{
			printf_shell(shell, "%.*s", get_file_information(argv[0])->size, executable_file_binary);
			free(executable_file_binary);
		}
		else printf_shell(shell, "Executable file \"%s\" is not found.\n", executable_file_name);
		// Discard file name and argv.
		free(executable_file_name);
		for(unsigned int argv_index = 0; argv_index < argc; argv_index++)free(argv[argv_index]);
		free(argv);
		return NULL;
	}
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

