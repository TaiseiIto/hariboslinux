// Usage: pack OUTPUT BOOTSECTOR [FILE] ...
//
// This program outputs Haribos Linux floppy disk raw image file named by the argument OUTPUT.
// The output includes the files specified by the arguments [FILE] ...

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const * const * const argv)
{
	// Haribos Linux floppy disk raw image file name
	char const *output_file_name;
	// Haribos Linux floppy disk boot sector binary file name
	char const *boot_sector_file_name;
	// file names included in the floppy disk
	char const * const *input_file_names;
	unsigned int num_of_input_files;
	// check argc
	if(argc < 3)
	{
		fprintf(stderr, "Usage: pack OUTPUT BOOTSECTOR [FILE] ...\n");
		return EXIT_FAILURE;
	}
	// print argv
	for(int argc_i = 0; argc_i < argc; argc_i++)printf("argv[%d] : %s\n", argc_i, argv[argc_i]);
	// decode argv
	output_file_name = argv[1];
	boot_sector_file_name = argv[2];
	input_file_names = argv + 3;
	num_of_input_files = argc - 3;
	printf("output file : %s\n", output_file_name);
	printf("boot sector bynari file : %s\n", boot_sector_file_name);
	for(unsigned int num_of_input_files_i = 0; num_of_input_files_i < num_of_input_files; num_of_input_files_i++)printf("input file [%u] : %s\n", num_of_input_files_i, input_file_names[num_of_input_files_i]);
	return EXIT_SUCCESS;
}

