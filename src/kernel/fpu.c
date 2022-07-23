#include "fpu.h"
#include "io.h"

Task *fpu_user_task = NULL;

Task const *get_fpu_user_task(void)
{
	return fpu_user_task;
}

void init_fpu(void)
{
	unsigned short control;
	prohibit_switch_task();
	fninit();
	fnstcw(&control);
	control &= ~FPU_STATUS_EXCEPTION_ZERO_DIVIDE;
	fldcw(&control);
	fpu_user_task = get_current_task();
	fpu_user_task->flags |= FPU_INITIALIZED;
	allow_switch_task();
}

void release_fpu(void)
{
	prohibit_switch_task();
	if(fpu_user_task == get_current_task())fpu_user_task = NULL;
	allow_switch_task();
}

void take_fpu(void)
{
	Task *current_task = get_current_task();
	if(fpu_user_task != current_task)
	{
		if(fpu_user_task)fnsave(&fpu_user_task->fpu_registers);
		if(current_task->flags & FPU_INITIALIZED)frstor(&current_task->fpu_registers);
		fpu_user_task = current_task;
	}
}

