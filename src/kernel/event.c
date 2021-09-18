#include "event.h"
#include "queue.h"

Queue * event_queue;

void create_event_queue(void)
{
	event_queue = create_queue(sizeof(Event));
}

Event const *dequeue_event(void)
{
	return dequeue(event_queue);
}

void enqueue_event(Event const *event)
{
	enqueue(event_queue, event);
}

