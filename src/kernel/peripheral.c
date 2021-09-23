#include "peripheral.h"
#include "serial.h"

void peripheral0_interrupt_handler(void)
{
	print_serial_polling("peripheral0 interrupt\n");
}

void peripheral1_interrupt_handler(void)
{
	print_serial_polling("peripheral1 interrupt\n");
}

void peripheral2_interrupt_handler(void)
{
	print_serial_polling("peripheral2 interrupt\n");
}

