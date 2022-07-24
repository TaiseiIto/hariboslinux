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

void alignment_check_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ALIGNMENT CHECK EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("ALIGNMENT CHECK EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void bound_range_exceeded_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "BOUND RANGE EXCEEDED EXCEPTION!!!\n");
	printf_serial("BOUND RANGE EXCEEDED EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void breakpoint_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "BREAKPOINT EXCEPTION!!!\n");
	printf_serial("BREAKPOINT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void control_protection_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "CONTROL PROTECTION EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("CONTROL_PROTECTION EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void coprocessor_segment_overrun_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
	printf_serial("COPROCESSOR SEGMENT OVERRRUN EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void debug_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "DEBUG EXCEPTION!!!\n");
	printf_serial("DEBUG EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void device_not_available_exception_handler(void)
{
	unsigned short fpu_status_word;
	switch_polling_serial_mode();
	if(get_cr0() & CR0_TASK_SWITCHED)
	{
		clts();
		take_fpu();
	}
	fpu_status_word = fnstsw();
	if(fpu_status_word & (FPU_STATUS_EXCEPTION_INVALID_OPERATION | FPU_STATUS_EXCEPTION_DENORMALIZED_OPERAND | FPU_STATUS_EXCEPTION_ZERO_DIVIDE | FPU_STATUS_EXCEPTION_OVERFLOW | FPU_STATUS_EXCEPTION_UNDERFLOW | FPU_STATUS_EXCEPTION_PRECISION))
	{
		fnclex();
		if(fpu_status_word & FPU_STATUS_EXCEPTION_ZERO_DIVIDE)
		{
			printf_shell(get_current_shell(), "FPU ZERO DIVIDE!!!\n");
			release_fpu();
			exit_application(-1, get_current_task()->task_status_segment.esp0);
		}
	}
	switch_interrupt_serial_mode();
}

void devide_by_zero_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "DEVIDE BY 0 EXCEPTION!!!\n");
	printf_serial("DEVIDE BY 0 EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void double_fault_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "DOUBLE FAULT EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("DOUBLE FAULT EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void fpu_error_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "FPU ERROR EXCEPTION!!!\n");
	printf_serial("FPU ERROR EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void general_protection_fault_exception_handler(unsigned int error_code)
{
	Shell *shell = get_current_shell();
	switch_polling_serial_mode();
	if(shell)
	{
		printf_shell(shell, "GENERAL PROTECTION FAULT!!!\n");
		printf_shell(shell, "ERROR CODE = %#010.8X\n", error_code);
		exit_application(-1, get_current_task()->task_status_segment.esp0);
	}
	else
	{
		printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "GENERAL PROTECTION FAULT EXCEPTION!!!\n");
		printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
		printf_serial("GENERAL PROTECTION FAULT EXCEPTION!!!\n");
		printf_serial("ERROR CODE = %#010.8X\n", error_code);
		kernel_panic();
	}
	switch_interrupt_serial_mode();
}

void invalid_opcode_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "INVALID OPCODE EXCEPTION!!!\n");
	printf_serial("INVALID OPCODE EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void invalid_TSS_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "INVALID TSS EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("INVALID TSS EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void kernel_panic(void)
{
	printf_serial("KERNEL PANIC!!!\n");
	cli();
	hlt();
}

void machine_check_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "MACHINE CHECK EXCEPTION!!!\n");
	printf_serial("MACHINE CHECK EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void non_maskable_interrupt_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "NON MASKABLE INTERRUPT!!!\n");
	printf_serial("NON MASKABLE INTERRUPT!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void overflow_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "OVERFLOW EXCEPTION!!!\n");
	printf_serial("OVERFLOW EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void page_fault_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "PAGE FAULT EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x0f(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X0F!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x16(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X16!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x17(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X17!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x18(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X18!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x19(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X19!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x1a(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X1A!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x1b(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X1B!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x1f(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0X1F!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x08(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x08!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x09(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x09!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void reserved_exception_handler0x0a(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "RESERVED EXCEPTION 0x0a!!!\n");
	printf_serial("PAGE FAULT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}


void security_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "SECURITY EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("SECURITY EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void segment_not_present_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "SEGMENT NOT PRESENT EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("SEGMENT NOT PRESENT EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void simd_floating_point_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "SIMD FLOATING POINT EXCEPTION!!!\n");
	printf_serial("SIMD FLOATING POINT EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void stack_segment_fault_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "STACK SEGMENT FAULT EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("STACK SEGMENT FAULT EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void virtualization_exception_handler(void)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "VIRTUALIZATION EXCEPTION!!!\n");
	printf_serial("VIRTUALIZATION EXCEPTION!!!\n");
	kernel_panic();
	switch_interrupt_serial_mode();
}

void vmm_communication_exception_handler(unsigned int error_code)
{
	switch_polling_serial_mode();
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "VMM COMMUNICATION EXCEPTION!!!\n");
	printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, exception_text_foreground_color, exception_text_background_color, "ERROR CODE = %#010.8X\n", error_code);
	printf_serial("VMM COMMUNICATION EXCEPTION!!!\n");
	printf_serial("ERROR CODE = %#010.8X\n", error_code);
	kernel_panic();
	switch_interrupt_serial_mode();
}

void x87_floating_point_exception_handler(void)
{
	unsigned short fpu_status_word;
	switch_polling_serial_mode();
	fpu_status_word = fnstsw();
	if(fpu_status_word & (FPU_STATUS_EXCEPTION_INVALID_OPERATION | FPU_STATUS_EXCEPTION_DENORMALIZED_OPERAND | FPU_STATUS_EXCEPTION_ZERO_DIVIDE | FPU_STATUS_EXCEPTION_OVERFLOW | FPU_STATUS_EXCEPTION_UNDERFLOW | FPU_STATUS_EXCEPTION_PRECISION))
	{
		fnclex();
		if(fpu_status_word & FPU_STATUS_EXCEPTION_ZERO_DIVIDE)
		{
			printf_shell(get_current_shell(), "FPU ZERO DIVIDE!!!\n");
			release_fpu();
			exit_application(-1, get_current_task()->task_status_segment.esp0);
		}
	}
	else kernel_panic();
	switch_interrupt_serial_mode();
}

