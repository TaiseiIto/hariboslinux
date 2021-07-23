// Usage: translator INPUT OUTPUT
//
// INPUT: font bitmap file
// OUTPUT: font data c code

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
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
	return EXIT_SUCCESS;
}

