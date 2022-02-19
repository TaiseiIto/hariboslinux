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
	unsigned char status;
	#define TASK_STATUS_SLEEP	0x00
	#define TASK_STATUS_WAIT	0x01
	#define TASK_STATUS_RUN		0x02
} Task;

void cli_task(void);
void cli_task_interrupt(void);
Task *create_task(void (*function)(void *), unsigned int stack_size);
Task *init_task(void);
void sleep_task(Task *task);
void start_task(Task *task);
void sti_task(void);
void sti_task_interrupt(void);
void switch_task(void);

#endif

