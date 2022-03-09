#ifndef _TIMER_H_
#define _TIMER_H_

#include "queue.h"

typedef struct _Timer
{
	unsigned long long estimated_count;
	unsigned long long interval_count;
	// if interval_count == 0, the Timer won't be repeated.
	Queue *event_queue;
	void *(*procedure)(void *arguments);
	void *arguments;
	void *returns;
	struct _Timer *previous;
	struct _Timer *next;
} Timer;

typedef struct _TimerEvent
{
	Timer *timer;
} TimerEvent;

void *call_timer_procedure(Timer *timer);
Timer *create_timer(unsigned long long estimated_count/*centisecond*/, unsigned long long interval_count/*centisecond*/, Queue *event_queue, void *(*procedure)(void *arguments), void *arguments, void *returns);
void delete_timer(Timer *timer);
void init_timer(void);
void timers_tick(void);

#endif

