#ifndef _PIT_H_
#define _PIT_H_

// Programmable Interval Timer

#include "queue.h"

void init_pit(Queue *interrupt_queue);
void pit_interrupt_handler(void);
void decode_pit_interrupt(void);

#endif

