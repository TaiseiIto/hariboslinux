#include "io.h"
#include "serial.h"

void main(void)
{
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	set_memory_size();
	while(1)io_hlt();
}

