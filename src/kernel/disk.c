#include "disk.h"
#include "serial.h"

void disk_interrupt_handler(void)
{
	print_serial_polling("disk interrupt\n");
}

