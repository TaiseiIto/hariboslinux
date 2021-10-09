#ifndef _TIMER_H_
#define _TIMER_H_

typedef struct _Timer
{
	unsigned long long estimated_count;
	unsigned long long interval_count;
	// if interval_count == 0, the Timer won't be repeated.
	struct _Timer *previous;
	struct _Timer *next;
} Timer;

typedef struct _TimerEvent
{
	Timer *timer;
} TimerEvent;

Timer *create_timer(unsigned long long estimated_count, unsigned long long interval_count);
void delete_timer(Timer *timer);
void init_timers(void);

#endif

