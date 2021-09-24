#include "stack.h"

void __stack_chk_fail(void)
{
	print_serial("__stack_chk_fail called!\n");
}

