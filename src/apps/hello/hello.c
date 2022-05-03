#include "system_call.h"

int main(void)
{
	system_call_write(1, "Hello, World!\n", 14);
	return 0;
}

