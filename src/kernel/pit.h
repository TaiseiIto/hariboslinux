#ifndef _PIT_H_
#define _PIT_H_

// Programmable Interval Timer

void init_pit(void);
void pit_interrupt_handler(void);
void decode_pit_interrupt(void);

#endif

