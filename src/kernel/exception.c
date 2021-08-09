#include "exception.h"
#include "serial.h"

void devide_by_zero_exception_handler(void)
{
	print_serial_polling("DEVIDE BY 0 EXCEPTION!!!\n");
}

