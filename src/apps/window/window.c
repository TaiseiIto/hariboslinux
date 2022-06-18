#include "dev.h"
#include "stdio.h"

int main(void)
{
	create_window("window.com", 0x0200, 0x0200, 0x0200, 0x0200);
	while(true)process_event();
	return 0;
}

