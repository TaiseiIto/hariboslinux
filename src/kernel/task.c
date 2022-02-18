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
	cli_task();
	new_task->previous = main_task->previous;
	new_task->next = main_task;
	main_task->previous->next = new_task;
	main_task->previous = new_task;
	sti_task();
	return new_task;
}

void init_task(void)
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
	main_task->task_status_segment.eip = (unsigned int)0x00006000;
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
	main_task->interrupt_prohibition_level = 1;
	main_task->previous = main_task;
	main_task->next = main_task;
	current_task = main_task;
	ltr(main_task->segment_selector);
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
	if(current_task != current_task->next)
	{
		current_task = current_task->next;
		ljmp(0, current_task->segment_selector);
	}
}

