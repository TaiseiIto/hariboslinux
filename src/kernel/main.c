#include "io.h"
#include "serial.h"

void main(void)
{
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	while(1)io_hlt();
}

