#include "peripheral.h"
#include "pic.h"
#include "serial.h"

void peripheral0_interrupt_handler(void)
{
	finish_interruption(IRQ_PERIPHERAL0);
	print_serial_polling("peripheral0 interrupt\n");
}

void peripheral1_interrupt_handler(void)
{
	finish_interruption(IRQ_PERIPHERAL1);
	print_serial_polling("peripheral1 interrupt\n");
}

void peripheral2_interrupt_handler(void)
{
	finish_interruption(IRQ_PERIPHERAL2);
	print_serial_polling("peripheral2 interrupt\n");
}

