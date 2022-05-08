#include "stdlib.h"
#include "stdio.h"
#include "string.h"

int main(int argc, char const * const * const argv)
{
	if(2 <= argc)
	{
		unsigned char buffer[0x10];
		unsigned int file_descriptor = fopen(argv[1], "r");
		size_t read_size;
		while((read_size = fread(buffer, sizeof(buffer[0]), _countof(buffer), file_descriptor)))
		{
			for(unsigned int i = 0; i < _countof(buffer); i++)printf(" %2.2X", buffer[i]);
			printf("\n");
		}
		fclose(file_descriptor);
		return 0;
	}
	else
	{
		printf("%s : Satisfy a file name.\n", argv[0]);
		return -1;
	}
}

