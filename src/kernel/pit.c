// Programmable Interval Timer

#include "event.h"
#include "io.h"
#include "limits.h"
#include "pic.h"
#include "pit.h"
#include "serial.h"
#include "task.h"
#include "timer.h"

#define PIT_CHANNEL0			0x0040
#define PIT_CHANNEL1			0x0041
#define PIT_CHANNEL2			0x0042
#define PIT_COMMAND			0x0043
#define PIT_COMMAND_SET_INTERVAL	0x34

Queue *pit_interrupt_queue;

const unsigned int pit_frequency = 1193182;
const unsigned int pit_interrupt_frequency = 100;

void init_pit(Queue *interrupt_queue)
{
	const unsigned int interval = pit_frequency / pit_interrupt_frequency + (pit_interrupt_frequency / 2 < pit_frequency % pit_interrupt_frequency);
	pit_interrupt_queue = interrupt_queue;
	outb(PIT_COMMAND, PIT_COMMAND_SET_INTERVAL);
	outb(PIT_CHANNEL0, (unsigned char)(interval & 0x000000ff));
	outb(PIT_CHANNEL0, (unsigned char)(interval >> CHAR_BIT & 0x000000ff));
}

void pit_interrupt_handler(void)
{
	Event event;
	static unsigned long long tick_count = 0;
	finish_interruption(IRQ_PIT);
	event.type = EVENT_TYPE_PIT_INTERRUPT;
	enqueue(pit_interrupt_queue, &event);
	switch_task();
}

void decode_pit_interrupt(void)
{
	timers_tick();
}

