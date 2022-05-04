#include "stack.h"
#include "stdio.h"
#include "stdlib.h"

void __stack_chk_fail(void)
{
	ERROR();
	exit(1);
}

