#include "dev.h"
#include "stdio.h"

int main(void)
{
	printf("window = %p\n", create_window("window.com", 0x0200, 0x0200, 0x0200, 0x0200));
	return 0;
}

