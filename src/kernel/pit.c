// Programmable Interval Timer

#include "event.h"
#include "io.h"
#include "limits.h"
#include "pic.h"
#include "pit.h"
#include "serial.h"

#define PIT_CHANNEL0			0x0040
#define PIT_CHANNEL1			0x0041
#define PIT_CHANNEL2			0x0042
#define PIT_COMMAND			0x0043
#define PIT_COMMAND_SET_INTERVAL	0x34

const unsigned int timer_frequency = 1193182;
const unsigned int timer_interrupt_frequency = 100;

void init_pit(void)
{
	const unsigned int interval = timer_frequency / timer_interrupt_frequency + (timer_interrupt_frequency / 2 < timer_frequency % timer_interrupt_frequency);
	outb(PIT_COMMAND, PIT_COMMAND_SET_INTERVAL);
	outb(PIT_CHANNEL0, (unsigned char)(interval & 0x000000ff));
	outb(PIT_CHANNEL0, (unsigned char)(interval >> CHAR_BIT & 0x000000ff));
}

void pit_interrupt_handler(void)
{
	Event event;
	finish_interruption(IRQ_PIT);
	event.type = EVENT_TYPE_PIT_INTERRUPT;
	enqueue_event(&event);
}

