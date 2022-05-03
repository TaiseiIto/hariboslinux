#include "chain_string.h"
#include "console.h"
#include "disk.h"
#include "io.h"
#include "memory.h"
#include "shell.h"
#include "stdio.h"
#include "string.h"

typedef struct _ComHeader
{
	unsigned int text_base;
	unsigned int rodata_base;
	unsigned int data_base;
	unsigned int bss_base;
	unsigned int common_base;
	unsigned int common_deletion_prevention_base;
	unsigned int heap_and_stack_base;
	unsigned int heap_and_stack_size;
} ComHeader;

typedef struct _CommandLineArgument
{
	ChainString *chain_string;
	struct _CommandLineArgument *previous;
	struct _CommandLineArgument *next;
} CommandLineArgument;

char const * const prompt = "> ";
ChainString *serial_console_input_string;
Shell *serial_shell = NULL;

char **create_argv(char const *command);
void command_task_procedure(CommandTaskArgument *arguments);

void clean_up_command_task(CommandTaskArgument *command_task_argument)
{
	ConsoleEvent *console_event;
	Event new_event;
	free(command_task_argument->com_file_binary);
	free(command_task_argument->com_file_name);
	for(unsigned int argv_index = 0; argv_index < command_task_argument->argc; argv_index++)free(command_task_argument->argv[argv_index]);
	free(command_task_argument->argv);
	printf_serial("Application return value = %d\n", ((CommandTaskReturn *)command_task_argument->task_return->task_return)->return_value);
	free(command_task_argument->task_return->task_return);
	switch(command_task_argument->shell->type)
	{
	case SHELL_TYPE_CONSOLE:
		// Send prompt event.
		console_event = malloc(sizeof(*console_event));
		console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
		new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
		new_event.event_union.sheet_user_defined_event.sheet = command_task_argument->shell->console->text_box->sheet;
		new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
		new_event.event_union.sheet_user_defined_event.any = console_event;
		enqueue(command_task_argument->shell->console->text_box->sheet->event_queue, &new_event);
		break;
	case SHELL_TYPE_SERIAL:
		print_serial(prompt);
		break;
	default:
		ERROR(); // Invalid shell type
		break;
	}
}

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

void command_task_procedure(CommandTaskArgument *arguments)
{
	ComHeader const *com_header = arguments->com_file_binary;
	void *application_memory = malloc(arguments->com_file_size + com_header->heap_and_stack_size);
	void *application_stack_floor;
	char *argv_string_writer;
	char **argv_element_writer;
	char ***argv_writer;
	unsigned int *argc_writer;
	void *writer;
	unsigned short executable_segment;
	unsigned short data_segment;
	// Register application memory to application task.
	get_current_task()->additionals = malloc(sizeof(CommandTaskAdditional));
	((CommandTaskAdditional *)get_current_task()->additionals)->application_memory = application_memory;
	// Copy executable file to application memory.
	memcpy(application_memory, arguments->com_file_binary, arguments->com_file_size);
	// Copy argv to application memory.
	writer = application_memory + arguments->com_file_size + com_header->heap_and_stack_size;
	for(unsigned int argi = 0; argi < arguments->argc; argi++)writer -= strlen(arguments->argv[argi]) + 1;
	argv_string_writer = writer;
	for(unsigned int argi = 0; argi < arguments->argc; argi++)writer -= sizeof(arguments->argv[argi]);
	argv_element_writer = writer;
	writer -= sizeof(arguments->argv);
	argv_writer = writer;
	writer -= sizeof(arguments->argc);
	argc_writer = writer;
	application_stack_floor = writer;
	*argc_writer = arguments->argc;
	*argv_writer = (char **)((unsigned int)argv_element_writer - (unsigned int)application_memory);
	for(unsigned int argi = 0; argi < arguments->argc; argi++)
	{
		argv_element_writer[argi] = (char *)((unsigned int)argv_string_writer - (unsigned int)application_memory);
		strcpy(argv_string_writer, arguments->argv[argi]);
		argv_string_writer += strlen(argv_string_writer) + 1;
	}
	// Alloc application segments.
	data_segment = alloc_segment(application_memory, arguments->com_file_size + com_header->heap_and_stack_size, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRIVILEGE);
	executable_segment = alloc_segment(application_memory, com_header->rodata_base, SEGMENT_DESCRIPTOR_READABLE | SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRIVILEGE);
	// Call application.
	((CommandTaskReturn *)arguments->task_return->task_return)->return_value = call_application
	(
		com_header->text_base/* eip */,
		EFLAGS_NOTHING | EFLAGS_INTERRUPT_FLAG/* eflags */,
		0/* eax */,
		0/* ecx */,
		0/* ebx */,
		0/* edx */,
		(unsigned int)application_stack_floor - (unsigned int)application_memory - 0x34/* esp */,
		(unsigned int)application_stack_floor - (unsigned int)application_memory/* ebp */,
		0/* esi */,
		0/* edi */,
		data_segment/* es */,
		executable_segment/* cs */,
		data_segment/* ss */,
		data_segment/* ds */,
		data_segment/* fs */,
		data_segment/* gs */,
		application_stack_floor/* application_stack_floor */,
		&get_current_task()->task_status_segment/* Task status segment */
	);
	// Clean up.
	free_segment(data_segment);
	free_segment(executable_segment);
	free(application_memory);
	free(get_current_task()->additionals);
	close_task(get_current_task());
}

Shell *create_shell(Console *console)
{
	Shell *shell;
	shell = malloc(sizeof(*shell));
	prohibit_switch_task();
	if(serial_shell)
	{
		shell->previous = serial_shell->previous;
		shell->next = serial_shell;
		serial_shell->previous->next = shell;
		serial_shell->previous = shell;
	}
	else
	{
		shell->previous = shell;
		shell->next = shell;
		serial_shell = shell;
	}
	allow_switch_task();
	if(console)
	{
		shell->event_queue = console->text_box->sheet->event_queue;
		shell->console = console;
		shell->type = SHELL_TYPE_CONSOLE;
	}
	else
	{
		shell->event_queue = get_current_task()->event_queue;
		shell->console = NULL;
		shell->type = SHELL_TYPE_SERIAL;
	}
	return shell;
}

void delete_shell(Shell *shell)
{
	printf_serial("Delete shell %p\n", shell);
	prohibit_switch_task();
	shell->previous->next = shell->next;
	shell->next->previous = shell->previous;
	allow_switch_task();
	free(shell);
}

void *execute_command(Shell *shell, char const *command)
{
	unsigned int argc;
	char **argv;
	// Create argv.
	argv = create_argv(command);
	if(argv)
	{
		char *com_file_name;
		void *com_file_binary;
		unsigned int com_file_size;
		// Count argc.
		for(argc = 0; argv[argc]; argc++);
		// Load a file specified by argv[0].
		com_file_name = create_format_char_array("%s.com", argv[0]);
		com_file_binary = load_file(com_file_name);
		com_file_size = get_file_information(com_file_name)->size;
		if(com_file_binary)
		{
			CommandTaskArgument *command_task_argument = malloc(sizeof(*command_task_argument));
			Task *command_task = create_task(get_current_task(), (void (*)(void *))command_task_procedure, 0x00010000, TASK_PRIORITY_USER);
			command_task_argument->com_file_name = com_file_name;
			command_task_argument->com_file_binary = com_file_binary;
			command_task_argument->com_file_size = com_file_size;
			command_task_argument->argc = argc;
			command_task_argument->argv = argv;
			command_task_argument->shell = shell;
			command_task_argument->task_return = malloc(sizeof(*command_task_argument->task_return));
			command_task_argument->task_return->task_type = TASK_TYPE_COMMAND;
			command_task_argument->task_return->task_return = malloc(sizeof(CommandTaskReturn));
			start_task(command_task, command_task_argument, command_task_argument->task_return, 1);
		}
		else
		{
			ConsoleEvent *console_event;
			Event new_event;
			printf_shell(shell, "Executable file \"%s\" is not found.\n", com_file_name);
			free(com_file_name);
			for(unsigned int argv_index = 0; argv_index < argc; argv_index++)free(argv[argv_index]);
			free(argv);
			switch(shell->type)
			{
			case SHELL_TYPE_CONSOLE:
				// Send prompt event.
				console_event = malloc(sizeof(*console_event));
				console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
				new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
				new_event.event_union.sheet_user_defined_event.sheet = shell->console->text_box->sheet;
				new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
				new_event.event_union.sheet_user_defined_event.any = console_event;
				enqueue(shell->console->text_box->sheet->event_queue, &new_event);
				break;
			case SHELL_TYPE_SERIAL:
				break;
			default:
				ERROR(); // Invalid shell type
				break;
			}
		}
		return NULL;
	}
	return NULL;
}

Shell *get_current_shell(void)
{
	Shell *shell = serial_shell;
	do
	{
		if(get_current_task()->parent == shell->event_queue->task)return shell;
		shell = shell->next;
	} while(shell != serial_shell);
	ERROR(); // The shell is not found.
	return NULL;
}

void init_shells(void)
{
	serial_console_input_string = create_chain_string("");
	serial_shell = create_shell(NULL);
}

void print_shell(Shell *shell, char const *string)
{
	for(; *string; string++)put_char_shell(shell, *string);
}

void printf_shell(Shell *shell, char const *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(((unsigned int)&format - (unsigned int)&shell) / sizeof(unsigned int));
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

