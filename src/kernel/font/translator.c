// Usage: translator INPUT OUTPUT
//
// INPUT: font bitmap file
// OUTPUT: font data c code

#include <stdio.h>
#include <stdlib.h>

#define _countof(array) (sizeof(array) / sizeof(array[0]))

#define CHAR_WIDTH 0x08
#define CHAR_HEIGHT 0x10
#define FOREGROUND '*'
#define BACKGROUND '-'

// width 8 pixels
// height 16 pixels
typedef struct
{
	unsigned char row[CHAR_HEIGHT];
} CharFont;

int main(int argc, char **argv)
{
	char input_char;
	char const *input_file_name;
	char const *output_file_name;
	CharFont map;
	FILE *input_file;
	FILE *output_file;
	unsigned char char_code;
	unsigned char flags = 0;
	#define INPUT_ZERO 0x01
	#define INPUT_CHAR_CODE1 0x02
	#define INPUT_CHAR_CODE2 0x04
	#define INPUT_MAP_BEGIN 0x08
	#define INPUT_MAP 0x10
	#define INPUT_MAP_NEWLINE 0x20
	unsigned char map_x;
	unsigned char map_y;
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
	fprintf(output_file, "CharFont const font[0x100] =\n");
	fprintf(output_file, "{\n");
	while((input_char = fgetc(input_file)) != EOF)
	{
		if(flags & INPUT_ZERO)
		{
			if(input_char == 'x' || input_char == 'X')flags |= INPUT_CHAR_CODE1;
			else fprintf(stderr, "%s is broken!\n", input_file_name);
			flags &= ~INPUT_ZERO;
		}
		else if(flags & INPUT_CHAR_CODE1)
		{
			if('0' <= input_char && input_char <= '9')char_code = input_char - '0';
			else if('a' <= input_char && input_char <= 'f')char_code = input_char - 'a' + 10;
			else if('A' <= input_char && input_char <= 'F')char_code = input_char - 'A' + 10;
			else fprintf(stderr, "%s is broken!\n", input_file_name);
			flags &= ~INPUT_CHAR_CODE1;
			flags |= INPUT_CHAR_CODE2;
		}
		else if(flags & INPUT_CHAR_CODE2)
		{
			char_code <<= 4;
			if('0' <= input_char && input_char <= '9')char_code += input_char - '0';
			else if('a' <= input_char && input_char <= 'f')char_code += input_char - 'a' + 10;
			else if('A' <= input_char && input_char <= 'F')char_code += input_char - 'A' + 10;
			else fprintf(stderr, "%s is broken!\n", input_file_name);
			flags &= ~INPUT_CHAR_CODE2;
			flags |= INPUT_MAP_BEGIN;
		}
		else if(flags & INPUT_MAP_BEGIN)
		{
			if(input_char != '\n')fprintf(stderr, "%s is broken!\n", input_file_name);
			flags &= ~INPUT_MAP_BEGIN;
			flags |= INPUT_MAP;
			map_x = 0;
			map_y = 0;
		}
		else if(flags & INPUT_MAP_NEWLINE)
		{
			switch(input_char)
			{
			case '\n':
				break;
			default:
				fprintf(stderr, "%s is broken!\n", input_file_name);
				break;
			}
			flags &= ~INPUT_MAP_NEWLINE;
		}
		else if(flags & INPUT_MAP)
		{
			map.row[map_y] >>= 1;
			switch(input_char)
			{
			case BACKGROUND:
				break;
			case FOREGROUND:
				map.row[map_y] += 0x80;
				break;
			default:
				fprintf(stderr, "%s is broken!\n", input_file_name);
				break;
			}
			if(CHAR_WIDTH <= ++map_x)
			{
				map_x = 0;
				flags |= INPUT_MAP_NEWLINE;
				if(CHAR_HEIGHT <= ++map_y)
				{
					fprintf(output_file, "\t{{");
					for(unsigned int row_i = 0; row_i < CHAR_HEIGHT; row_i++)fprintf(output_file, row_i == CHAR_HEIGHT - 1 ? "%#04x" : "%#04x, ", map.row[row_i]);
					fprintf(output_file, char_code == 0xff ? "}}\n" : "}},\n");
					map_y = 0;
					flags &= ~INPUT_MAP;
					flags &= ~INPUT_MAP_NEWLINE;
				}
			}
		}
		else switch(input_char)
		{
		case '0':
			flags |= INPUT_ZERO;
			break;
		default:
			break;
		}
	}
	fprintf(output_file, "};\n");
	fprintf(output_file, "\n");
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

