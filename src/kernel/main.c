#include "io.h"
#include "serial.h"

void main(void)
{
	print_serial_polling("Hello, kernel.bin!\n");
	while(1)io_hlt();
}

