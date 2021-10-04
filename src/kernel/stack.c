#include "serial.h"
#include "stack.h"

void __stack_chk_fail(void)
{
	ERROR_MESSAGE();
}

