#ifndef _TASK_H_
#define _TASK_H_

typedef struct _TaskStatusSegment
{
	unsigned int link;
	unsigned int esp0;
	unsigned int ss0;
	unsigned int esp1;
	unsigned int ss1;
	unsigned int esp2;
	unsigned int ss2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;
	unsigned int cs;
	unsigned int ss;
	unsigned int ds;
	unsigned int fs;
	unsigned int gs;
	unsigned int ldtr;
	unsigned int io;
} TaskStatusSegment;

typedef struct _Task
{
	TaskStatusSegment task_status_segment;
	void *stack;
	unsigned int interrupt_prohibition_level;
	struct _Task *previous;
	struct _Task *next;
	unsigned short segment_selector;
} Task;

void cli_task(void);
void cli_task_interrupt(void);
Task *create_task(void (*function)(void *), unsigned int stack_size);
void init_task(void);
void sti_task(void);
void sti_task_interrupt(void);
void test_task(void);

#endif

