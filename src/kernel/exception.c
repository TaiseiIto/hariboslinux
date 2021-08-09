#include "exception.h"
#include "serial.h"

void bound_range_exceeded_exception_handler(void)
{
	print_serial_polling("BOUND RANGE EXCEEDED EXCEPTION!!!\n");
}

void devide_by_zero_exception_handler(void)
{
	print_serial_polling("DEVIDE BY 0 EXCEPTION!!!\n");
}

