#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char *combine_argv(int argc, char const * const * const argv);

int main(int argc, char const * const * const argv)
{
	char *input_string = combine_argv(argc - 1, argv + 1);
	printf("%s\n", input_string);
	free(input_string);
	return 0;
}

char *combine_argv(int argc, char const * const * const argv)
{
	char *combined_string;
	char *writer;
	unsigned int sum_length = 0;
	for(int argi = 0; argi < argc; argi++)sum_length += strlen(argv[argi]);
	combined_string = malloc(sum_length + 1);
	writer = combined_string;
	for(int argi = 0; argi < argc; argi++)
	{
		strcpy(writer, argv[argi]);
		writer += strlen(argv[argi]);
	}
	return combined_string;
}

