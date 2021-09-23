#include "parallel.h"
#include "serial.h"

void lpt1_interrupt_handler(void)
{
	print_serial_polling("LPT1 interrupt\n");
}

void lpt2_interrupt_handler(void)
{
	print_serial_polling("LPT2 interrupt\n");
}

