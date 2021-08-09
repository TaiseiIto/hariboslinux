#include "exception.h"
#include "serial.h"

void bound_range_exceeded_exception_handler(void)
{
	print_serial_polling("BOUND RANGE EXCEEDED EXCEPTION!!!\n");
}

void coprocessor_segment_overrun_exception_handler(void)
{
	print_serial_polling("COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
}

void device_not_available_exception_handler(void)
{
	print_serial_polling("DEVICE NOT AVAILABLE EXCEPTION!!!\n");
}

void devide_by_zero_exception_handler(void)
{
	print_serial_polling("DEVIDE BY 0 EXCEPTION!!!\n");
}

void double_fault_exception_handler(void)
{
	print_serial_polling("DOUBLE FAULT EXCEPTION!!!\n");
}

void invalid_opcode_exception_handler(void)
{
	print_serial_polling("INVALID OPCODE EXCEPTION!!!\n");
}

void invalid_TSS_exception_handler(void)
{
	print_serial_polling("INVALID TSS EXCEPTION!!!\n");
}

