#include "event.h"
#include "queue.h"

Queue *event_queue;

void create_event_queue(Task *task)
{
	event_queue = create_queue(sizeof(Event), task);
}

Event const *dequeue_event(void)
{
	return dequeue(event_queue);
}

void enqueue_event(Event const *event)
{
	enqueue(event_queue, event);
}

