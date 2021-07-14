#include "io.h"

void main(void)
{
	print_serial("Hello, kernel.bin!\n");
	while(1)io_hlt();
}

