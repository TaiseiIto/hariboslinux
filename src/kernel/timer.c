#include "serial.h"
#include "timer.h"

void timer_interrupt_handler(void)
{
	print_serial_polling("timer interrupt\n");
}

