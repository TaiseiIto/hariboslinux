#include "stdio.h"
#include "stdlib.h"

int main(void)
{
	unsigned int key[4] =
	{
		0x123,
		0x234,
		0x345,
		0x456
	};
	init_mersenne_twister(key, _countof(key));
	for(unsigned int i = 0; i < 1000; i++)
	{
		printf("%10lu ", rand());
		if(i % 5 == 4)printf("\n");
	}
	return 0;
}

