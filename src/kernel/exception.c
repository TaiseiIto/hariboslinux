#include "exception.h"
#include "font.h"
#include "fpu.h"
#include "graphic.h"
#include "io.h"
#include "pic.h"
#include "serial.h"
#include "shell.h"

//						{red ,green, blue,alpha}
const Color exception_text_background_color =	{0x00, 0x00, 0x00, 0xff};
const Color exception_text_foreground_color = 	{0xff, 0xff, 0xff, 0xff};

void kernel_panic(void);

void alignment_check_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ALIGNMENT CHECK EXCEPTION!!!\n");
	print_serial("ALIGNMENT CHECK EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void bound_range_exceeded_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "BOUND RANGE EXCEEDED EXCEPTION!!!\n");
	print_serial("BOUND RANGE EXCEEDED EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void breakpoint_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "BREAKPOINT EXCEPTION!!!\n");
	print_serial("BREAKPOINT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void coprocessor_segment_overrun_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
	print_serial("COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void debug_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "DEBUG EXCEPTION!!!\n");
	print_serial("DEBUG EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void device_not_available_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_serial("DEVICE NOT AVAILABLE EXCEPTION!!!\n");
	clts();
	take_fpu();
	switch_interrupt_serial_mode();
}

void devide_by_zero_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "DEVIDE BY 0 EXCEPTION!!!\n");
	print_serial("DEVIDE BY 0 EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void double_fault_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "DOUBLE FAULT EXCEPTION!!!\n");
	print_serial("DOUBLE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void fpu_error_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "FPU ERROR EXCEPTION!!!\n");
	print_serial("FPU ERROR EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void general_protection_fault_exception_handler(void)
{
	Shell *shell = get_current_shell();
	switch_polling_serial_mode();
	if(shell)
	{
		printf_shell(shell, "GENERAL PROTECTION FAULT!!!\n");
		exit_application(-1, get_current_task()->task_status_segment.esp0);
	}
	else
	{
		printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "GENERAL PROTECTION FAULT EXCEPTION!!!\n");
		print_serial("GENERAL PROTECTION FAULT EXCEPTION!!!\n");
		kernel_panic();
	}
	switch_interrupt_serial_mode();
}

void invalid_opcode_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "INVALID OPCODE EXCEPTION!!!\n");
	print_serial("INVALID OPCODE EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void invalid_TSS_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "INVALID TSS EXCEPTION!!!\n");
	print_serial("INVALID TSS EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void kernel_panic(void)
{
	print_serial("KERNEL PANIC!!!\n");
	cli();
	hlt();
}

void machine_check_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "MACHINE CHECK EXCEPTION!!!\n");
	print_serial("MACHINE CHECK EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void non_maskable_interrupt_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "NON MASKABLE INTERRUPT!!!\n");
	print_serial("NON MASKABLE INTERRUPT!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void overflow_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "OVERFLOW EXCEPTION!!!\n");
	print_serial("OVERFLOW EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void page_fault_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "PAGE FAULT EXCEPTION!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x00(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x00!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x01(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x01!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x02(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x02!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x03(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x03!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x04(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x04!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x05(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x05!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x06(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x06!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x07(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x07!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x08(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x08!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x09(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x09!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x0a(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x0a!!!\n");
	print_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}


void security_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "SECURITY EXCEPTION!!!\n");
	print_serial("SECURITY EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void segment_not_present_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "SEGMENT NOT PRESENT EXCEPTION!!!\n");
	print_serial("SEGMENT NOT PRESENT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void simd_floating_point_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "SIMD FLOATING POINT EXCEPTION!!!\n");
	print_serial("SIMD FLOATING POINT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void stack_segment_fault_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "STACK SEGMENT FAULT EXCEPTION!!!\n");
	print_serial("STACK SEGMENT FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void virtualization_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "VIRTUALIZATION EXCEPTION!!!\n");
	print_serial("VIRTUALIZATION EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void x87_floating_point_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "X87 FLOATING POINT EXCEPTION!!!\n");
	print_serial("X87 FLOATING POINT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

