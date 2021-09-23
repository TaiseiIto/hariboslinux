#include "serial.h"
#include "timer.h"

void real_time_clock_interrupt_handler(void)
{
	print_serial_polling("real time clock interrupt\n");
}

void timer_interrupt_handler(void)
{
	print_serial_polling("timer interrupt\n");
}

