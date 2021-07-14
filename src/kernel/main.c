#include "io.h"

void main(void)
{
	new_line_serial();
	print_serial("Hello, kernel.bin!\n");
	while(1)io_hlt();
}

