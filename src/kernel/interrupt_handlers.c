#include "interrupt_handlers.h"
#include "serial.h"

void interrupt_handler0x0f(void)
{
	print_serial_polling("int 0x0f\n");
}

void interrupt_handler0x15(void)
{
	print_serial_polling("int 0x15\n");
}

void interrupt_handler0x16(void)
{
	print_serial_polling("int 0x16\n");
}

void interrupt_handler0x17(void)
{
	print_serial_polling("int 0x17\n");
}

void interrupt_handler0x18(void)
{
	print_serial_polling("int 0x18\n");
}

void interrupt_handler0x19(void)
{
	print_serial_polling("int 0x19\n");
}

void interrupt_handler0x1a(void)
{
	print_serial_polling("int 0x1a\n");
}

void interrupt_handler0x1b(void)
{
	print_serial_polling("int 0x1b\n");
}

void interrupt_handler0x1c(void)
{
	print_serial_polling("int 0x1c\n");
}

void interrupt_handler0x1d(void)
{
	print_serial_polling("int 0x1d\n");
}

void interrupt_handler0x1f(void)
{
	print_serial_polling("int 0x1f\n");
}

