// Usage: translator INPUT OUTPUT
//
// INPUT: font bitmap file
// OUTPUT: font data c code

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	char input_char;
	char const *input_file_name;
	char const *output_file_name;
	FILE *input_file;
	FILE *output_file;
	unsigned char char_code;
	unsigned char flags = 0;
	#define INPUT_ZERO 0x01
	#define INPUT_CHAR_CODE1 0x02
	#define INPUT_CHAR_CODE2 0x04
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
	// write output file contents
	fprintf(output_file, "#include \"font.h\"\n");
	fprintf(output_file, "\n");
	fprintf(output_file, "CharFont _font =\n");
	fprintf(output_file, "{\n");
	while((input_char = fgetc(input_file)) != EOF)
	{
		if(flags & INPUT_CHAR_CODE1)
		{
			if('0' <= input_char && input_char <= '9')char_code = input_char - '0';
			else if('a' <= input_char && input_char <= 'f')char_code = input_char - 'a';
			else if('A' <= input_char && input_char <= 'F')char_code = input_char - 'A';
			else fprintf(stderr, "%s is broken!\n", input_file_name);
			flags &= ~INPUT_CHAR_CODE1;
			flags |= INPUT_CHAR_CODE2;
		}
		else if(flags & INPUT_CHAR_CODE2)
		{
			char_code <<= 4;
			if('0' <= input_char && input_char <= '9')char_code += input_char - '0';
			else if('a' <= input_char && input_char <= 'f')char_code += input_char - 'a';
			else if('A' <= input_char && input_char <= 'F')char_code += input_char - 'A';
			else fprintf(stderr, "%s is broken!\n", input_file_name);
			printf("char code %#04X\n", char_code);
			flags &= ~INPUT_CHAR_CODE2;
		}
		else switch(input_char)
		{
		case '0':
			flags |= INPUT_ZERO;
			break;
		case 'x':
		case 'X':
			if(flags & INPUT_ZERO)flags |= INPUT_CHAR_CODE1;
			flags &= ~INPUT_ZERO;
			break;
		default:
			break;
		}
	}
	fprintf(output_file, "};\n");
	fprintf(output_file, "\n");
	fprintf(output_file, "CharFont const * const font = &_font;");
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

