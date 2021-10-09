#include "event.h"
#include "memory.h"
#include "task.h"
#include "timer.h"

Timer *next_estimated_timer;
unsigned long long tick_count;

Timer *create_timer(unsigned long long estimated_count, unsigned long long interval_count)
{
	Timer *new_timer = (Timer *)malloc(sizeof(*new_timer));
	new_timer->estimated_count = tick_count + estimated_count;
	new_timer->interval_count = interval_count;
	cli_task();
	if(next_estimated_timer)
	{
		Timer *previous_timer;
		for(previous_timer = next_estimated_timer; previous_timer->next; previous_timer = previous_timer->next)if(new_timer->estimated_count < previous_timer->next->estimated_count)break;
		if(previous_timer == next_estimated_timer && new_timer->estimated_count < next_estimated_timer->estimated_count)
		{
			new_timer->next = next_estimated_timer;
			next_estimated_timer->previous = new_timer;
			new_timer->previous = NULL;
			next_estimated_timer = new_timer;
		}
		else
		{
			new_timer->next = previous_timer->next;
			if(new_timer->next)new_timer->next->previous = new_timer;
			new_timer->previous = previous_timer;
			previous_timer->next = new_timer;
		}
	}
	else
	{
		new_timer->previous = NULL;
		new_timer->next = NULL;
		next_estimated_timer = new_timer;
	}
	sti_task();
	return new_timer;
}

void delete_timer(Timer *timer)
{
	cli_task();
	if(next_estimated_timer == timer)next_estimated_timer = timer->next;
	if(timer->previous)timer->previous->next = timer->next;
	if(timer->next)timer->next->previous = timer->previous;
	sti_task();
	free(timer);
}

void init_timer(void)
{
	next_estimated_timer = NULL;
	tick_count = 0;
}

void timers_tick(void)
{
	tick_count++;
	cli_task();
	if(next_estimated_timer)while(next_estimated_timer->estimated_count <= tick_count)
	{
		Event event;
		event.type = EVENT_TYPE_TIMER_EVENT;
		event.event_union.timer_event.timer = next_estimated_timer;
		enqueue_event(&event);
		if(next_estimated_timer->interval_count)
		{
			Timer *repeating_timer = next_estimated_timer;
			next_estimated_timer = next_estimated_timer->next;
			if(repeating_timer->previous)repeating_timer->previous->next = repeating_timer->next;
			if(repeating_timer->next)repeating_timer->next->previous = repeating_timer->previous;
			repeating_timer->previous = NULL;
			repeating_timer->next = NULL;
			repeating_timer->estimated_count += repeating_timer->interval_count;
			if(next_estimated_timer)
			{
				Timer *previous_timer;
				for(previous_timer = next_estimated_timer; previous_timer->next; previous_timer = previous_timer->next)if(repeating_timer->estimated_count < previous_timer->next->estimated_count)break;
				if(previous_timer == next_estimated_timer && repeating_timer->estimated_count < next_estimated_timer->estimated_count)
				{
					repeating_timer->next = next_estimated_timer;
					next_estimated_timer->previous = repeating_timer;
					repeating_timer->previous = NULL;
					next_estimated_timer = repeating_timer;
				}
				else
				{
					repeating_timer->next = previous_timer->next;
					if(repeating_timer->next)repeating_timer->next->previous = repeating_timer;
					repeating_timer->previous = previous_timer;
					previous_timer->next = repeating_timer;
				}
			}
			else next_estimated_timer = repeating_timer;
		}
		else next_estimated_timer = next_estimated_timer->next;
	}
	sti_task();
}

