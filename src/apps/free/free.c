#include "stdio.h"

int main(void)
{
	unsigned int free_memory_size;
	unsigned int file_descriptor = fopen("free.dev", "r");
	fread(&free_memory_size, sizeof(free_memory_size), 1, file_descriptor);
	printf("free memory space %#010.8x bytes\n", free_memory_size);
	fclose(file_descriptor);
	return 0;
}

