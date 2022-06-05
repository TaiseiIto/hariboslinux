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

typedef struct _WindowCommandCreateArguments
{
	char *title;
	short x;
	short y;
	unsigned short width;
	unsigned short height;
} WindowCommandCreateArguments;

typedef union _WindowCommandArguments
{
	WindowCommandCreateArguments create;
} WindowCommandArguments;

typedef struct _WindowCommand
{
	WindowCommandArguments arguments;
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
	unsigned int file_descriptor = fopen("window.dev", "wr");
	unsigned int window;
	command.type = WINDOW_COMMAND_CREATE;
	command.arguments.create.title = "test";
	command.arguments.create.x = 0x0200;
	command.arguments.create.y = 0x0200;
	command.arguments.create.width = 0x0200;
	command.arguments.create.height = 0x0200;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fread(&window, sizeof(window), 1, file_descriptor);
	fclose(file_descriptor);
	return window;
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

