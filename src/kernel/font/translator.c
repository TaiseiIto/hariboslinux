// Usage: translator INPUT OUTPUT
//
// INPUT: font bitmap file
// OUTPUT: font data c code

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char const *input_file_name;
	char const *output_file_name;
	FILE *input_file;
	FILE *output_file;
	// "translator", "INPUT" and "OUTPUT"
	unsigned int const num_of_necessary_args = 3;
	// check argc
	if((unsigned int)argc < num_of_necessary_args)
	{
		fprintf(stderr, "Usage: translator INPUT OUTPUT\n");
		return EXIT_FAILURE;
	}
	// print argv
	for(int argc_i = 0; argc_i < argc; argc_i++)printf("argv[%d] : %s\n", argc_i, argv[argc_i]);
	// decode argv
	input_file_name = argv[1];
	output_file_name = argv[2];
	printf("input file name: %s\n", input_file_name);
	printf("output file name: %s\n", output_file_name);
	// open files
	if((input_file = fopen(input_file_name, "r")) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", input_file_name);
		return EXIT_FAILURE;
	}
	if((output_file = fopen(output_file_name, "w")) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", output_file_name);
		return EXIT_FAILURE;
	}
	// close files
	if(fclose(input_file) == EOF)
	{
		fprintf(stderr, "Can't close %s\n", input_file_name);
		return EXIT_FAILURE;
	}
	if(fclose(output_file) == EOF)
	{
		fprintf(stderr, "Can't close %s\n", output_file_name);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

