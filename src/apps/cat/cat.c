#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const * const * const argv)
{
	if(2 <= argc)
	{
		char buffer[0x200];
		unsigned int file_descriptor = fopen(argv[1], "r");
		size_t read_size;
		while((read_size = fread(buffer, sizeof(buffer[0]), _countof(buffer), file_descriptor)))printf("%.*s", read_size, buffer);
		fclose(file_descriptor);
		return 0;
	}
	else
	{
		printf("%s : Satisfy a file name.\n", argv[0]);
		return -1;
	}
}

