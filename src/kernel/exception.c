#include "exception.h"
#include "serial.h"

void alignment_check_exception_handler(void)
{
	print_serial_polling("ALIGNMENT CHECK EXCEPTION!!!\n");
}

void bound_range_exceeded_exception_handler(void)
{
	print_serial_polling("BOUND RANGE EXCEEDED EXCEPTION!!!\n");
}

void breakpoint_exception_handler(void)
{
	print_serial_polling("BREAKPOINT EXCEPTION!!!\n");
}

void coprocessor_segment_overrun_exception_handler(void)
{
	print_serial_polling("COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
}

void debug_exception_handler(void)
{
	print_serial_polling("DEBUG EXCEPTION!!!\n");
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

void fpu_error_exception_handler(void)
{
	print_serial_polling("FPU ERROR EXCEPTION!!!\n");
}

void general_protection_fault_exception_handler(void)
{
	print_serial_polling("GENERAL PROTEXTION FAULT EXCEPTION!!!\n");
}

void invalid_opcode_exception_handler(void)
{
	print_serial_polling("INVALID OPCODE EXCEPTION!!!\n");
}

void invalid_TSS_exception_handler(void)
{
	print_serial_polling("INVALID TSS EXCEPTION!!!\n");
}

void machine_check_exception_handler(void)
{
	print_serial_polling("MACHINE CHECK EXCEPTION!!!\n");
}

void page_fault_exception_handler(void)
{
	print_serial_polling("PAGE FAULT EXCEPTION!!!\n");
}

void security_exception_handler(void)
{
	print_serial_polling("SECURITY EXCEPTION!!!\n");
}

void segment_not_present_exception_handler(void)
{
	print_serial_polling("SEGMENT NOT PRESENT EXCEPTION!!!\n");
}

void simd_floating_point_exception_handler(void)
{
	print_serial_polling("SIMD FLOATING POINT EXCEPTION!!!\n");
}

void stack_segment_fault_exception_handler(void)
{
	print_serial_polling("STACK SEGMENT FAULT EXCEPTION!!!\n");
}

void virtualization_exception_handler(void)
{
	print_serial_polling("VIRTUALIZATION EXCEPTION!!!\n");
}

void x87_floating_point_exception_handler(void)
{
	print_serial_polling("X87 FLOATING POINT EXCEPTION!!!\n");
}

