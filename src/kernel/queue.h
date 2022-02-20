#ifndef _QUEUE_H_
#define _QUEUE_H_

struct _Queue;

#include "stddef.h"
#include "task.h"

typedef struct _QueueElement
{
	void *data;
	struct _QueueElement *next;
} QueueElement;

typedef struct _Queue
{
	size_t element_size;
	void *io; // This is needed for dynamic size element enqueue/dequeue.
	unsigned int number_of_elements;
	QueueElement *read_head;
	QueueElement *write_head;
	struct _Task *task;
} Queue;

Queue *create_event_queue(struct _Task *task);
Queue *create_queue(size_t element_size, struct _Task *task);
void *dequeue(Queue *queue);
void delete_queue(Queue *queue);
void enqueue(Queue *queue, void const *data);

#endif

