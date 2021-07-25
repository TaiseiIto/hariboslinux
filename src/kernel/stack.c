#include "stack.h"

void __stack_chk_fail(void)
{
	print_serial_polling("__stack_chk_fail called!\n");
}

