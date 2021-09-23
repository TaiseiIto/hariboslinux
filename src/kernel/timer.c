#include "pic.h"
#include "serial.h"
#include "timer.h"

void real_time_clock_interrupt_handler(void)
{
	finish_interruption(IRQ_REAL_TIME_CLOCK);
	print_serial_polling("real time clock interrupt\n");
}

void timer_interrupt_handler(void)
{
	finish_interruption(IRQ_TIMER);
	print_serial_polling("timer interrupt\n");
}

