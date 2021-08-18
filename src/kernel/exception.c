#include "exception.h"
#include "io.h"
#include "serial.h"

void kernel_panic(void);

void alignment_check_exception_handler(void)
{
	print_serial_polling("ALIGNMENT CHECK EXCEPTION!!!\n");
	kernel_panic();
}

void bound_range_exceeded_exception_handler(void)
{
	print_serial_polling("BOUND RANGE EXCEEDED EXCEPTION!!!\n");
	kernel_panic();
}

void breakpoint_exception_handler(void)
{
	print_serial_polling("BREAKPOINT EXCEPTION!!!\n");
	kernel_panic();
}

void coprocessor_segment_overrun_exception_handler(void)
{
	print_serial_polling("COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
	kernel_panic();
}

void debug_exception_handler(void)
{
	print_serial_polling("DEBUG EXCEPTION!!!\n");
	kernel_panic();
}

void device_not_available_exception_handler(void)
{
	print_serial_polling("DEVICE NOT AVAILABLE EXCEPTION!!!\n");
	kernel_panic();
}

void devide_by_zero_exception_handler(void)
{
	print_serial_polling("DEVIDE BY 0 EXCEPTION!!!\n");
	kernel_panic();
}

void double_fault_exception_handler(void)
{
	print_serial_polling("DOUBLE FAULT EXCEPTION!!!\n");
	kernel_panic();
}

void fpu_error_exception_handler(void)
{
	print_serial_polling("FPU ERROR EXCEPTION!!!\n");
	kernel_panic();
}

void general_protection_fault_exception_handler(void)
{
	print_serial_polling("GENERAL PROTEXTION FAULT EXCEPTION!!!\n");
	kernel_panic();
}

void invalid_opcode_exception_handler(void)
{
	print_serial_polling("INVALID OPCODE EXCEPTION!!!\n");
	kernel_panic();
}

void invalid_TSS_exception_handler(void)
{
	print_serial_polling("INVALID TSS EXCEPTION!!!\n");
	kernel_panic();
}

void kernel_panic(void)
{
	print_serial_polling("KERNEL PANIC!!!\n");
	cli();
	hlt();
}

void machine_check_exception_handler(void)
{
	print_serial_polling("MACHINE CHECK EXCEPTION!!!\n");
	kernel_panic();
}

void non_maskable_interrupt_handler(void)
{
	print_serial_polling("NON MASKABLE INTERRUPT!!!\n");
	kernel_panic();
}

void overflow_exception_handler(void)
{
	print_serial_polling("OVERFLOW EXCEPTION!!!\n");
	kernel_panic();
}

void page_fault_exception_handler(void)
{
	print_serial_polling("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
}

void security_exception_handler(void)
{
	print_serial_polling("SECURITY EXCEPTION!!!\n");
	kernel_panic();
}

void segment_not_present_exception_handler(void)
{
	print_serial_polling("SEGMENT NOT PRESENT EXCEPTION!!!\n");
	kernel_panic();
}

void simd_floating_point_exception_handler(void)
{
	print_serial_polling("SIMD FLOATING POINT EXCEPTION!!!\n");
	kernel_panic();
}

void stack_segment_fault_exception_handler(void)
{
	print_serial_polling("STACK SEGMENT FAULT EXCEPTION!!!\n");
	kernel_panic();
}

void virtualization_exception_handler(void)
{
	print_serial_polling("VIRTUALIZATION EXCEPTION!!!\n");
	kernel_panic();
}

void x87_floating_point_exception_handler(void)
{
	print_serial_polling("X87 FLOATING POINT EXCEPTION!!!\n");
	kernel_panic();
}

