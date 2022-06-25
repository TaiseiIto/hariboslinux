#ifndef _TASK_H_
#define _TASK_H_

struct _Task;

#include "gdt.h"
#include "queue.h"

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

struct _TaskLevel;

typedef struct _Task
{
	TaskStatusSegment task_status_segment;
	SegmentDescriptor *ldt;
	void *stack;
	void *arguments;
	void *additionals;
	void *returns;
	struct _Queue *event_queue;
	struct _Task *parent;
	struct _Task *previous;
	struct _Task *next;
	struct _TaskLevel *task_level;
	unsigned int interrupt_prohibition_level;
	unsigned int switch_prohibition_level;
	unsigned short segment_selector;
	unsigned char elapsed_time;		// centi second
	unsigned char flags;
	#define TASK_FLAG_SWITCH_PENDING	0x01
	unsigned char occupancy_time;		// centi second
	unsigned char status;
	#define TASK_STATUS_SLEEP		0x00
	#define TASK_STATUS_WAIT		0x01
	#define TASK_STATUS_RUN			0x02
} Task;

typedef struct _TaskLevel
{
	struct _Task *current_task;
	struct _TaskLevel *higher;
	struct _TaskLevel *lower;
	int priority;
	#define TASK_PRIORITY_KERNEL		0x40000000
	#define TASK_PRIORITY_SHELL		0x20000000
	#define TASK_PRIORITY_USER		0x00000000
	#define TASK_PRIORITY_IDLE		-0x40000000
} TaskLevel;

typedef struct _TaskReturn
{
	unsigned char task_type;
	#define TASK_TYPE_CONSOLE	0x00
	#define TASK_TYPE_COMMAND	0x01
	#define TASK_TYPE_TEST		0x02
	void *task_return;
} TaskReturn;

typedef struct _TaskDeletionRequestEvent
{
	Task *task;
} TaskDeletionRequestEvent;

typedef struct _TaskDeletionResponseEvent
{
	Task *task;
	void *arguments;
	void *returns;
	unsigned short segment_selector;
} TaskDeletionResponseEvent;

extern Task main_task;

void allow_switch_task(void);
void cli_task(void);
void cli_task_interrupt(void);
void close_task(Task *task);
void continue_task(Task *task);
Task *create_task(Task *parent, void (*procedure)(void *), unsigned int stack_size, int priority);
Task *get_current_task(void);
Task *init_task(void);
void prohibit_switch_task(void);
void sleep_task(Task *task);
void start_task(Task *task, void *arguments, void *returns, unsigned char occupancy_time);
void sti_task(void);
void sti_task_interrupt(void);
void switch_task(void);

#endif

