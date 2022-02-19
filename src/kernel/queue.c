#include "memory.h"
#include "queue.h"
#include "string.h"
#include "task.h"

Queue *create_queue(size_t element_size, Task *task)
{
	Queue *queue = malloc(sizeof(*queue));
	queue->element_size = element_size;
	queue->io = malloc(element_size);
	queue->number_of_elements = 0;
	queue->read_head = NULL;
	queue->write_head = NULL;
	queue->task = task;
	return queue;
}

void *dequeue(Queue *queue)
{
	if(queue->read_head)
	{
		QueueElement *dequeued_element = queue->read_head;
		cli_task();
		memcpy(queue->io, dequeued_element->data, queue->element_size);
		queue->read_head = queue->read_head->next;
		if(!queue->read_head)queue->write_head = NULL; // dequeue last element
		queue->number_of_elements--;
		free(dequeued_element->data);
		free(dequeued_element);
		sti_task();
		return queue->io;
	}
	else return NULL; // no element!
}

void delete_queue(Queue *queue)
{
	while(queue->read_head)dequeue(queue);
	free(queue->io);
	free(queue);
}

void enqueue(Queue *queue, void const *data)
{
	QueueElement *new_element;
	cli_task();
	new_element = malloc(sizeof(*new_element));
	new_element->data = malloc(queue->element_size);
	memcpy(new_element->data, data, queue->element_size);
	new_element->next = NULL;
	if(queue->write_head)queue->write_head->next = new_element;
	queue->write_head = new_element;
	if(!queue->read_head)queue->read_head = new_element;
	queue->number_of_elements++;
	sti_task();
	if(queue->task)if(queue->task->status == TASK_STATUS_SLEEP)start_task(queue->task);
}

