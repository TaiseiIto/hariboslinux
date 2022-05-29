#include "stdio.h"

int main(void)
{
	unsigned int window = fopen("window.dev", "rw");
	printf("Hello, Window!\n");
	return 0;
}

