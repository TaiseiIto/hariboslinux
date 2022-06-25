#ifndef _PIT_H_
#define _PIT_H_

// Programmable Interval Timer

#include "queue.h"

#define PIT_CHANNEL0			0x0040
#define PIT_CHANNEL1			0x0041
#define PIT_CHANNEL2			0x0042
#define PIT_COMMAND			0x0043
#define PIT_COMMAND_SET_INTERVAL	0x34

extern const unsigned int pit_interrupt_frequency;

void init_pit(Queue *interrupt_queue);
void pit_interrupt_handler(void);
void decode_pit_interrupt(void);

#endif

