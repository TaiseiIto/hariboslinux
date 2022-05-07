#include "stdio.h"

int main(void)
{
	unsigned int file_descriptor = fopen("", "r");
	printf("file_descriptor = %p\n", file_descriptor);
	return 0;
}

