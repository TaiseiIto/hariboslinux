#include "font.h"
#include "graphic.h"
#include "io.h"
#include "serial.h"
#include "task.h"

Task *current_task;
Task main_task;

void cli_task(void)
{
	if(!(current_task->interrupt_prohibition_level++))cli();
}

void cli_task_interrupt(void)
{
	current_task->interrupt_prohibition_level++;
}

void init_task(void)
{
	main_task.interrupt_prohibition_level = 1;
	current_task = &main_task;
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

