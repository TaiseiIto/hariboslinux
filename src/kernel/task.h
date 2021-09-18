#ifndef _TASK_H_
#define _TASK_H_

typedef struct _Task
{
	unsigned int interrupt_prohibition_level;
} Task;

void cli_task(void);
void cli_task_interrupt(void);
void init_task(void);
void sti_task(void);
void sti_task_interrupt(void);

#endif

