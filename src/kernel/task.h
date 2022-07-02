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

typedef struct _FPURegisters
{
	unsigned short coltrol;
	#define FPU_CONTROL_EXCEPTION_MASK_INVALID_OPERATION	0x0001
	#define FPU_CONTROL_EXCEPTION_MASK_DENORMALIZED_OPERAND	0x0002
	#define FPU_CONTROL_EXCEPTION_MASK_ZERO_DIVIDE		0x0004
	#define FPU_CONTROL_EXCEPTION_MASK_OVERFLOW		0x0008
	#define FPU_CONTROL_EXCEPTION_MASK_UNDERFLOW		0x0010
	#define FPU_CONTROL_EXCEPTION_MASK_PRECISION		0x0020
	#define FPU_CONTROL_PRECISION_24_BITS			0x0000
	#define FPU_CONTROL_PRECISION_53_BITS			0x0200
	#define FPU_CONTROL_PRECISION_64_BITS			0x0300
	#define FPU_CONTROL_ROUND_TOWARD_NEAREST		0x0000
	#define FPU_CONTROL_ROUND_DOWN				0x0400
	#define FPU_CONTROL_ROUND_UP				0x0800
	#define FPU_CONTROL_ROUND_TOWARD_ZERO			0x0c00
	unsigned short reserved0;
	unsigned short status;
	#define FPU_STATUS_EXCEPTION_INVALID_OPERATION		0x0001
	#define FPU_STATUS_EXCEPTION_DENORMALIZED_OPERAND	0x0002
	#define FPU_STATUS_EXCEPTION_ZERO_DIVIDE		0x0004
	#define FPU_STATUS_EXCEPTION_OVERFLOW			0x0008
	#define FPU_STATUS_EXCEPTION_UNDERFLOW			0x0010
	#define FPU_STATUS_EXCEPTION_PRECISION			0x0020
	#define FPU_STATUS_STACK_FAULT				0x0040
	#define FPU_STATUS_EXCEPTION_SUMMARY			0x0080
	#define FPU_STATUS_C0					0x0100
	#define FPU_STATUS_C1					0x0200
	#define FPU_STATUS_C2					0x0400
	#define FPU_STATUS_TOP					0x3800
	#define FPU_STATUS_C3					0x4000
	#define FPU_STATUS_BUSY					0x8000
	unsigned short reserved1;
	unsigned short tag;
	#define FPU_TAG_REGISTER_0_VALID	0x0000
	#define FPU_TAG_REGISTER_0_ZERO		0x0001
	#define FPU_TAG_REGISTER_0_SPECIAL	0x0002
	#define FPU_TAG_REGISTER_0_EMPTY	0x0003
	#define FPU_TAG_REGISTER_1_VALID	0x0000
	#define FPU_TAG_REGISTER_1_ZERO		0x0004
	#define FPU_TAG_REGISTER_1_SPECIAL	0x0008
	#define FPU_TAG_REGISTER_1_EMPTY	0x000c
	#define FPU_TAG_REGISTER_2_VALID	0x0000
	#define FPU_TAG_REGISTER_2_ZERO		0x0010
	#define FPU_TAG_REGISTER_2_SPECIAL	0x0020
	#define FPU_TAG_REGISTER_2_EMPTY	0x0030
	#define FPU_TAG_REGISTER_3_VALID	0x0000
	#define FPU_TAG_REGISTER_3_ZERO		0x0040
	#define FPU_TAG_REGISTER_3_SPECIAL	0x0080
	#define FPU_TAG_REGISTER_3_EMPTY	0x00c0
	#define FPU_TAG_REGISTER_4_VALID	0x0000
	#define FPU_TAG_REGISTER_4_ZERO		0x0100
	#define FPU_TAG_REGISTER_4_SPECIAL	0x0200
	#define FPU_TAG_REGISTER_4_EMPTY	0x0300
	#define FPU_TAG_REGISTER_5_VALID	0x0000
	#define FPU_TAG_REGISTER_5_ZERO		0x0400
	#define FPU_TAG_REGISTER_5_SPECIAL	0x0800
	#define FPU_TAG_REGISTER_5_EMPTY	0x0c00
	#define FPU_TAG_REGISTER_6_VALID	0x0000
	#define FPU_TAG_REGISTER_6_ZERO		0x1000
	#define FPU_TAG_REGISTER_6_SPECIAL	0x2000
	#define FPU_TAG_REGISTER_6_EMPTY	0x3000
	#define FPU_TAG_REGISTER_7_VALID	0x0000
	#define FPU_TAG_REGISTER_7_ZERO		0x4000
	#define FPU_TAG_REGISTER_7_SPECIAL	0x8000
	#define FPU_TAG_REGISTER_7_EMPTY	0xc000
	unsigned short reserved2;
	unsigned int instruction_pointer_offset;
	unsigned short instruction_pointer_selector;
	unsigned short opcode;
	unsigned int data_pointer_offset;
	unsigned short data_pointer_selector;
	unsigned short reserved3;
	unsigned char data_registers[8][10];
} FPURegisters;

struct _TaskLevel;

typedef struct _Task
{
	TaskStatusSegment task_status_segment;
	FPURegisters fpu_registers;
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
	#define FPU_INITIALIZED			0x02
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

