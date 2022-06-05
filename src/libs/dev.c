#include "dev.h"
#include "stdio.h"

typedef struct _ConsoleCommand
{
	unsigned char type;
	#define CONSOLE_COMMAND_CLEAR	0x00
} ConsoleCommand;

typedef struct _MemoryCommand
{
	unsigned char type;
	#define MEMORY_COMMAND_FREE	0x00
} MemoryCommand;

typedef struct _WindowCommand
{
	unsigned char type;
	#define WINDOW_COMMAND_CREATE	0x00
} WindowCommand;

void clear_console(void)
{
	ConsoleCommand command;
	unsigned int file_descriptor = fopen("console.dev", "w");
	command.type = CONSOLE_COMMAND_CLEAR;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
}

unsigned int create_window(void)
{
	WindowCommand command;
	unsigned int file_descriptor = fopen("window.dev", "w");
	command.type = WINDOW_COMMAND_CREATE;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
	return 0;
}

unsigned int get_free_memory_space_size(void)
{
	MemoryCommand command;
	unsigned int file_descriptor = fopen("memory.dev", "wr");
	unsigned int free_memory_space_size;
	command.type = MEMORY_COMMAND_FREE;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fread(&free_memory_space_size, sizeof(free_memory_space_size), 1, file_descriptor);
	fclose(file_descriptor);
	return free_memory_space_size;
}

