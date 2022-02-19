#include "font.h"
#include "graphic.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "stdbool.h"
#include "task.h"

Task *current_task;
Task *main_task;

void test_task_function(void *args);

void cli_task(void)
{
	if(!(current_task->interrupt_prohibition_level++))cli();
}

void cli_task_interrupt(void)
{
	current_task->interrupt_prohibition_level++;
}

Task *create_task(void (*function)(void *), unsigned int stack_size)
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
	new_task->task_status_segment.eip = (unsigned int)function;
	new_task->task_status_segment.eflags = EFLAGS_NOTHING | EFLAGS_INTERRUPT_FLAG;
	new_task->task_status_segment.eax = 0;
	new_task->task_status_segment.ecx = 0;
	new_task->task_status_segment.edx = 0;
	new_task->task_status_segment.ebx = 0;
	new_task->task_status_segment.esp = (unsigned int)new_task->stack + stack_size - sizeof(/*argument of the function*/void *);
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
	new_task->segment_selector = set_segment(&new_task->task_status_segment, sizeof(new_task->task_status_segment), SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_ACCESSED);
	new_task->status = TASK_STATUS_SLEEP;
	cli_task();
	new_task->previous = main_task->previous;
	new_task->next = main_task;
	main_task->previous->next = new_task;
	main_task->previous = new_task;
	sti_task();
	return new_task;
}

Task *init_task(void)
{
	main_task = malloc(sizeof(*main_task));
	main_task->stack = MEMORY_MAP_KERNEL_STACK_BEGIN;
	main_task->task_status_segment.link = 0;
	main_task->task_status_segment.esp0 = 0;
	main_task->task_status_segment.ss0 = 0;
	main_task->task_status_segment.esp1 = 0;
	main_task->task_status_segment.ss1 = 0;
	main_task->task_status_segment.esp2 = 0;
	main_task->task_status_segment.ss2 = 0;
	main_task->task_status_segment.cr3 = 0;
	main_task->task_status_segment.eip = (unsigned int)MEMORY_MAP_KERNEL_BIN_BEGIN;
	main_task->task_status_segment.eflags = EFLAGS_NOTHING | EFLAGS_INTERRUPT_FLAG;
	main_task->task_status_segment.eax = 0;
	main_task->task_status_segment.ecx = 0;
	main_task->task_status_segment.edx = 0;
	main_task->task_status_segment.ebx = 0;
	main_task->task_status_segment.esp = (unsigned int)main_task->stack;
	main_task->task_status_segment.ebp = main_task->task_status_segment.ebp;
	main_task->task_status_segment.esi = 0;
	main_task->task_status_segment.edi = 0;
	main_task->task_status_segment.es = whole_memory_segment_selector;
	main_task->task_status_segment.cs = kernel_code_segment_selector;
	main_task->task_status_segment.ss = whole_memory_segment_selector;
	main_task->task_status_segment.ds = whole_memory_segment_selector;
	main_task->task_status_segment.fs = whole_memory_segment_selector;
	main_task->task_status_segment.gs = whole_memory_segment_selector;
	main_task->task_status_segment.ldtr = 0x00000000;
	main_task->task_status_segment.io = 0x40000000;
	main_task->segment_selector = set_segment(&main_task->task_status_segment, sizeof(main_task->task_status_segment), SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_ACCESSED);
	main_task->status = TASK_STATUS_RUN;
	main_task->interrupt_prohibition_level = 1;
	main_task->previous = main_task;
	main_task->next = main_task;
	current_task = main_task;
	ltr(main_task->segment_selector);
	return main_task;
}

void sleep_task(Task *task)
{
	cli_task();
	switch(task->status)
	{
	case TASK_STATUS_SLEEP:
		// The task is already sleeping.
		ERROR_MESSAGE();
		break;
	case TASK_STATUS_WAIT:
		task->status = TASK_STATUS_SLEEP;
		break;
	case TASK_STATUS_RUN:
		if(task == current_task)
		{
			bool next_task_found = false;
			for(Task *next_task = current_task->next; next_task != current_task; next_task = next_task->next)if(next_task->status == TASK_STATUS_WAIT)
			{
				next_task_found = true;
				task->status = TASK_STATUS_SLEEP;
				next_task->status = TASK_STATUS_RUN;
				current_task = next_task;
				ljmp(0, current_task->segment_selector);
				break;
			}
			if(!next_task_found)
			{
				sti_task();
				hlt();
				return;
			}
		}
		else ERROR_MESSAGE(); // Task status contradiction
		break;
	default:
		// Invalid task status
		ERROR_MESSAGE();
		break;
	}
	sti_task();
}

void start_task(Task *task)
{
	cli_task();
	switch(task->status)
	{
	case TASK_STATUS_SLEEP:
		task->status = TASK_STATUS_WAIT;
		break;
	case TASK_STATUS_WAIT:
	case TASK_STATUS_RUN:
		// The task has already started.
		ERROR_MESSAGE();
		break;
	default:
		// Invalid task status
		ERROR_MESSAGE();
		break;
	}
	sti_task();
}

void sti_task(void)
{
	if(current_task->interrupt_prohibition_level)
	{
		if(!(--current_task->interrupt_prohibition_level))sti();
	}
	else ERROR_MESSAGE(); // double sti error!
}

void sti_task_interrupt(void)
{
	if(current_task->interrupt_prohibition_level)current_task->interrupt_prohibition_level--;
	else ERROR_MESSAGE(); // double sti error!
}

void switch_task(void)
{
	cli_task();
	for(Task *next_task = current_task->next; next_task != current_task; next_task = next_task->next)if(next_task->status == TASK_STATUS_WAIT)
	{
		current_task->status = TASK_STATUS_WAIT;
		next_task->status = TASK_STATUS_RUN;
		current_task = next_task;
		ljmp(0, current_task->segment_selector);
		break;
	}
	sti_task();
}

