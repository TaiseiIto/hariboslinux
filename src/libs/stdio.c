#include "stdio.h"
#include "string.h"
#include "system_call.h"

int fputs(char const *string, unsigned int file_descriptor)
{
	char const * const new_line = "\n";
	system_call_write(file_descriptor, string, strlen(string));
	system_call_write(file_descriptor, new_line, strlen(new_line));
	return 0;
}

int puts(char const *string)
{
	return fputs(string, STDOUT);
}

