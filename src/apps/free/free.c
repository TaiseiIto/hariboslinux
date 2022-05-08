#include "stdio.h"
#include "string.h"

int main(void)
{
	char const * const command = "free";
	unsigned int free_memory_size;
	unsigned int file_descriptor = fopen("memory.dev", "wr");
	fwrite(command, sizeof(command[0]), strlen(command), file_descriptor);
	fread(&free_memory_size, sizeof(free_memory_size), 1, file_descriptor);
	printf("free memory space %#010.8x bytes\n", free_memory_size);
	fclose(file_descriptor);
	return 0;
}

