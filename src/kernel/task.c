#include "font.h"
#include "graphic.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "stdbool.h"
#include "task.h"

TaskLevel *current_task_level;
TaskLevel *highest_task_level;
TaskLevel *lowest_task_level;

void allow_switch_task(void)
{
	if(current_task_level->current_task->switch_prohibition_level)
	{
		if(!--current_task_level->current_task->switch_prohibition_level)if(current_task_level->current_task->flags & TASK_FLAG_SWITCH_PENDING)
		{
			current_task_level->current_task->flags &= ~TASK_FLAG_SWITCH_PENDING;
			switch_task();
		}
	}
	else ERROR(); // already allowed.
}

void cli_task(void)
{
	if(!(current_task_level->current_task->interrupt_prohibition_level++))cli();
}

void cli_task_interrupt(void)
{
	current_task_level->current_task->interrupt_prohibition_level++;
}

void close_task(Task *task)
{
	bool next_task_found = false;
	Task *next_task;
	TaskLevel *next_task_level;
	cli_task();
	if(task == current_task_level->current_task)
	{
		for(next_task_level = highest_task_level; next_task_level; next_task_level = next_task_level->lower)
		{
			for(next_task = next_task_level->current_task->next; next_task != next_task_level->current_task; next_task = next_task->next)if(next_task->status == TASK_STATUS_WAIT)
			{
				next_task_found = true;
				break;
			}
			if(next_task_found)break;
		}
		if(next_task_found)ERROR(); // Can't close task!
	}
	if(task->previous == task && task->next == task)// Close the task level
	{
		if(task->task_level->higher)task->task_level->higher->lower = task->task_level->lower;
		if(task->task_level->lower)task->task_level->lower->higher = task->task_level->higher;
		if(task->task_level == highest_task_level)highest_task_level = highest_task_level->lower;
		if(task->task_level == lowest_task_level)lowest_task_level = lowest_task_level->higher;
		free(task->task_level);
	}
	else if(task->previous != task && task->next != task)// exclude the task
	{
		if(task->task_level->current_task == task)task->task_level->current_task = task->previous;
		task->previous->next = task->next;
		task->next->previous = task->previous;
	}
	else ERROR(); // Task structure is broken!
	// free the task
	delete_queue(task->event_queue);
	free(task->stack);
	free(task);
	if(next_task_found)
	{
		next_task->status = TASK_STATUS_RUN;
		current_task_level = next_task_level;
		current_task_level->current_task = next_task;
		ljmp(0, current_task_level->current_task->segment_selector);
	}
	sti_task();
}

void continue_task(Task *task)
{
	prohibit_switch_task();
	switch(task->status)
	{
	case TASK_STATUS_SLEEP:
		task->status = TASK_STATUS_WAIT;
		if(current_task_level->priority < task->task_level->priority)switch_task();
		break;
	case TASK_STATUS_WAIT:
	case TASK_STATUS_RUN:
		// The task has already started.
		ERROR();
		break;
	default:
		// Invalid task status
		ERROR();
		break;
	}
	allow_switch_task();
}

Task *create_task(Task *parent, void (*procedure)(void *), unsigned int stack_size, int priority)
{
	Task *new_task = malloc(sizeof(*new_task));
	new_task->stack = malloc(stack_size);
	new_task->task_status_segment.link = 0;
	new_task->task_status_segment.esp0 = 0;
	new_task->task_status_segment.ss0 = 0;
	new_task->task_status_segment.esp1 = 0;
	new_task->task_status_segment.ss1 = 0;
	new_task->task_status_segment.esp2 = 0;
	new_task->task_status_segment.ss2 = 0;
	new_task->task_status_segment.cr3 = 0;
	new_task->task_status_segment.eip = (unsigned int)procedure;
	new_task->task_status_segment.eflags = EFLAGS_NOTHING | EFLAGS_INTERRUPT_FLAG;
	new_task->task_status_segment.eax = 0;
	new_task->task_status_segment.ecx = 0;
	new_task->task_status_segment.edx = 0;
	new_task->task_status_segment.ebx = 0;
	new_task->task_status_segment.esp = (unsigned int)new_task->stack + stack_size - sizeof(/*exclusive stack floor*/void *) - sizeof(/*argument of the procedure*/void *);
	new_task->task_status_segment.ebp = new_task->task_status_segment.ebp;
	new_task->task_status_segment.esi = 0;
	new_task->task_status_segment.edi = 0;
	new_task->task_status_segment.es = whole_memory_segment_selector;
	new_task->task_status_segment.cs = kernel_code_segment_selector;
	new_task->task_status_segment.ss = whole_memory_segment_selector;
	new_task->task_status_segment.ds = whole_memory_segment_selector;
	new_task->task_status_segment.fs = whole_memory_segment_selector;
	new_task->task_status_segment.gs = whole_memory_segment_selector;
	new_task->task_status_segment.ldtr = 0x00000000;
	new_task->task_status_segment.io = 0x40000000;
	new_task->segment_selector = alloc_segment(&new_task->task_status_segment, sizeof(new_task->task_status_segment), SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_ACCESSED);
	new_task->elapsed_time = 0;
	new_task->flags = 0;
	new_task->occupancy_time = 0;
	new_task->status = TASK_STATUS_SLEEP;
	new_task->interrupt_prohibition_level = 0;
	new_task->switch_prohibition_level = 0;
	prohibit_switch_task();
	new_task->parent = parent;
	new_task->task_level = NULL;
	for(TaskLevel *task_level = highest_task_level; task_level; task_level = task_level->lower)
	{
		if(task_level->priority == priority)
		{
			new_task->task_level = task_level;
			new_task->previous = task_level->current_task->previous;
			new_task->next = task_level->current_task;
			task_level->current_task->previous->next = new_task;
			task_level->current_task->previous = new_task;
			break;
		}
		else if(task_level->priority < priority)
		{
			TaskLevel *new_task_level = malloc(sizeof(*new_task_level));
			new_task_level->current_task = new_task;
			new_task_level->priority = priority;
			new_task_level->higher = task_level->higher;
			new_task_level->lower = task_level;
			if(task_level->higher)task_level->higher->lower = new_task_level;
			else highest_task_level = new_task_level;
			task_level->higher = new_task_level;
			new_task->task_level = new_task_level;
			new_task->previous = new_task;
			new_task->next = new_task;
			break;
		}
	}
	if(!new_task->task_level)
	{
		TaskLevel *new_task_level = malloc(sizeof(*new_task_level));
		new_task_level->current_task = new_task;
		new_task_level->priority = priority;
		new_task_level->higher = lowest_task_level;
		new_task_level->lower = NULL;
		lowest_task_level->lower = new_task_level;
		lowest_task_level = new_task_level;
		new_task->task_level = new_task_level;
		new_task->previous = new_task;
		new_task->next = new_task;
	}
	allow_switch_task();
	return new_task;
}

Task const *get_current_task(void)
{
	return current_task_level->current_task;
}

Task *init_task(void)
{
	current_task_level = malloc(sizeof(*current_task_level));
	highest_task_level = current_task_level;
	lowest_task_level = current_task_level;
	current_task_level->higher = NULL;
	current_task_level->lower = NULL;
	current_task_level->priority = TASK_PRIORITY_KERNEL;
	current_task_level->current_task = malloc(sizeof(*current_task_level->current_task));
	current_task_level->current_task->stack = MEMORY_MAP_KERNEL_STACK_BEGIN;
	current_task_level->current_task->task_status_segment.link = 0;
	current_task_level->current_task->task_status_segment.esp0 = 0;
	current_task_level->current_task->task_status_segment.ss0 = 0;
	current_task_level->current_task->task_status_segment.esp1 = 0;
	current_task_level->current_task->task_status_segment.ss1 = 0;
	current_task_level->current_task->task_status_segment.esp2 = 0;
	current_task_level->current_task->task_status_segment.ss2 = 0;
	current_task_level->current_task->task_status_segment.cr3 = 0;
	current_task_level->current_task->task_status_segment.eip = (unsigned int)MEMORY_MAP_KERNEL_BIN_BEGIN;
	current_task_level->current_task->task_status_segment.eflags = EFLAGS_NOTHING | EFLAGS_INTERRUPT_FLAG;
	current_task_level->current_task->task_status_segment.eax = 0;
	current_task_level->current_task->task_status_segment.ecx = 0;
	current_task_level->current_task->task_status_segment.edx = 0;
	current_task_level->current_task->task_status_segment.ebx = 0;
	current_task_level->current_task->task_status_segment.esp = (unsigned int)current_task_level->current_task->stack;
	current_task_level->current_task->task_status_segment.ebp = current_task_level->current_task->task_status_segment.ebp;
	current_task_level->current_task->task_status_segment.esi = 0;
	current_task_level->current_task->task_status_segment.edi = 0;
	current_task_level->current_task->task_status_segment.es = whole_memory_segment_selector;
	current_task_level->current_task->task_status_segment.cs = kernel_code_segment_selector;
	current_task_level->current_task->task_status_segment.ss = whole_memory_segment_selector;
	current_task_level->current_task->task_status_segment.ds = whole_memory_segment_selector;
	current_task_level->current_task->task_status_segment.fs = whole_memory_segment_selector;
	current_task_level->current_task->task_status_segment.gs = whole_memory_segment_selector;
	current_task_level->current_task->task_status_segment.ldtr = 0x00000000;
	current_task_level->current_task->task_status_segment.io = 0x40000000;
	current_task_level->current_task->segment_selector = alloc_segment(&current_task_level->current_task->task_status_segment, sizeof(current_task_level->current_task->task_status_segment), SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_ACCESSED);
	current_task_level->current_task->parent = NULL;
	current_task_level->current_task->previous = current_task_level->current_task;
	current_task_level->current_task->next = current_task_level->current_task;
	current_task_level->current_task->task_level = current_task_level;
	current_task_level->current_task->elapsed_time = 0;
	current_task_level->current_task->flags = 0;
	current_task_level->current_task->occupancy_time = 1;
	current_task_level->current_task->status = TASK_STATUS_RUN;
	current_task_level->current_task->interrupt_prohibition_level = 1;
	current_task_level->current_task->switch_prohibition_level = 0;
	ltr(current_task_level->current_task->segment_selector);
	return current_task_level->current_task;
}

void prohibit_switch_task(void)
{
	if(!++current_task_level->current_task->switch_prohibition_level)ERROR(); // switch prohibition level is broken.
}

void sleep_task(Task *task)
{
	cli_task();
	switch(task->status)
	{
	case TASK_STATUS_SLEEP:
		// The task is already sleeping.
		ERROR();
		break;
	case TASK_STATUS_WAIT:
		task->status = TASK_STATUS_SLEEP;
		break;
	case TASK_STATUS_RUN:
		if(task == current_task_level->current_task)
		{
			bool next_task_found = false;
			for(TaskLevel *next_task_level = highest_task_level; next_task_level; next_task_level = next_task_level->lower)
			{
				Task *next_task = next_task_level->current_task;
				do
				{
					next_task = next_task->next;
					if(next_task->status == TASK_STATUS_WAIT)
					{
						next_task_found = true;
						task->status = TASK_STATUS_SLEEP;
						next_task->status = TASK_STATUS_RUN;
						current_task_level = next_task_level;
						current_task_level->current_task = next_task;
						ljmp(0, current_task_level->current_task->segment_selector);
						break;
					}
				} while(next_task != next_task_level->current_task);
				if(next_task_found)break;
			}
			if(!next_task_found)
			{
				sti_task();
				hlt();
				return;
			}
		}
		else ERROR(); // Task status contradiction
		break;
	default:
		// Invalid task status
		ERROR();
		break;
	}
	sti_task();
}

void start_task(Task *task, void *arguments, unsigned char occupancy_time)
{
	prohibit_switch_task();
	switch(task->status)
	{
	case TASK_STATUS_SLEEP:
		task->occupancy_time = occupancy_time;
		task->elapsed_time = 0;
		*(void **)(task->task_status_segment.esp + sizeof(void *)) = arguments;
		task->status = TASK_STATUS_WAIT;
		break;
	case TASK_STATUS_WAIT:
	case TASK_STATUS_RUN:
		// The task has already started.
		ERROR();
		break;
	default:
		// Invalid task status
		ERROR();
		break;
	}
	allow_switch_task();
}

void sti_task(void)
{
	if(current_task_level->current_task->interrupt_prohibition_level)
	{
		if(!(--current_task_level->current_task->interrupt_prohibition_level))sti();
	}
	else ERROR(); // double sti error!
}

void sti_task_interrupt(void)
{
	if(current_task_level->current_task->interrupt_prohibition_level)current_task_level->current_task->interrupt_prohibition_level--;
	else ERROR(); // double sti error!
}

void switch_task(void)
{
	cli_task();
	if(current_task_level->current_task->occupancy_time <= ++current_task_level->current_task->elapsed_time)
	{
		if(!current_task_level->current_task->switch_prohibition_level)
		{
			bool next_task_found = false;
			for(TaskLevel *next_task_level = highest_task_level; next_task_level; next_task_level = next_task_level->lower)
			{
				Task *next_task = next_task_level->current_task;
				do
				{
					next_task = next_task->next;
					if(next_task->status == TASK_STATUS_WAIT)
					{
						next_task_found = true;
						current_task_level->current_task->status = TASK_STATUS_WAIT;
						next_task->status = TASK_STATUS_RUN;
						current_task_level = next_task_level;
						current_task_level->current_task = next_task;
						ljmp(0, current_task_level->current_task->segment_selector);
						break;
					}
				} while(next_task != next_task_level->current_task);
				if(next_task_found)break;
			}
		}
		else current_task_level->current_task->flags |= TASK_FLAG_SWITCH_PENDING;
	}
	sti_task();
}

