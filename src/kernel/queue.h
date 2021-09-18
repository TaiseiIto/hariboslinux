#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "stddef.h"

typedef struct _QueueElement
{
	struct _QueueElement *next;
	void *data;
} QueueElement;

typedef struct _Queue
{
	QueueElement *write_head;
	QueueElement *read_head;
	void *io; // This is needed for dynamic size element enqueue/dequeue.
	size_t element_size;
} Queue;

Queue *create_queue(size_t element_size);
void *dequeue(Queue *queue);
void destroy_queue(Queue *queue);
void enqueue(Queue *queue, void const *data);

#endif

