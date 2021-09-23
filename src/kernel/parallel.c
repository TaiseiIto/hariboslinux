#include "parallel.h"
#include "pic.h"
#include "serial.h"

void lpt1_interrupt_handler(void)
{
	finish_interruption(IRQ_LPT1);
	print_serial_polling("LPT1 interrupt\n");
}

void lpt2_interrupt_handler(void)
{
	finish_interruption(IRQ_LPT2);
	print_serial_polling("LPT2 interrupt\n");
}

