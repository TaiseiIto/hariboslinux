#include "chain_string.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "system_call.h"

int fputs(char const *string, unsigned int file_descriptor)
{
	char const * const new_line = "\n";
	system_call_write(file_descriptor, string, strlen(string));
	system_call_write(file_descriptor, new_line, strlen(new_line));
	return 0;
}

int printf(const char *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(((unsigned int)&format - (unsigned int)&format) / sizeof(unsigned int));
	char *output_string = create_char_array_from_chain_string(output_chain_string);
	int length = strlen(output_string);
	system_call_write(STDOUT, output_string, length);
	free(output_string);
	delete_chain_string(output_chain_string);
	return length;
}

int puts(char const *string)
{
	return fputs(string, STDOUT);
}

