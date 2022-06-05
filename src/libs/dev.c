#include "dev.h"
#include "stdio.h"

void clear_console(void)
{
	ConsoleCommand console_command;
	unsigned int file_descriptor = fopen("console.dev", "w");
	console_command.type = CONSOLE_COMMAND_CLEAR;
	fwrite(&console_command, sizeof(console_command), 1, file_descriptor);
	fclose(file_descriptor);
}

