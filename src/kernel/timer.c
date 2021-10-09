#include "memory.h"
#include "timer.h"

Timer *next_timer;
unsigned long long tick_counter;

Timer *create_timer(unsigned long long estimated_count, unsigned long long interval_count)
{
	return NULL;
}

void delete_timer(Timer *timer)
{
}

void init_timer(void)
{
	next_timer = NULL;
	tick_counter = 0;
}

void timers_tick(void)
{
	tick_counter++;
}

