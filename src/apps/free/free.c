#include "dev.h"
#include "stdio.h"
#include "string.h"

int main(void)
{
	printf("free memory space %#010.8x bytes\n", get_free_memory_space_size());
	return 0;
}

