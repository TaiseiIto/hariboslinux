#include "memory.h"
#include "timer.h"

Timer *next_timer;

Timer *create_timer(unsigned long long estimated_count, unsigned long long interval_count)
{
	return NULL;
}

void delete_timer(Timer *timer)
{
}

void init_timers(void)
{
	next_timer = NULL;
}

