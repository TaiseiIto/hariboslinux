#include "font.h"
#include "graphic.h"
#include "io.h"
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
	Color background_color;
	Color foreground_color;
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "current_task = %p", current_task);
	if(current_task->interrupt_prohibition_level)if(!(--current_task->interrupt_prohibition_level))sti();
	else return; // double sti error!
}

void sti_task_interrupt(void)
{
	if(current_task->interrupt_prohibition_level)current_task->interrupt_prohibition_level--;
	else return; // double sti error!
}

