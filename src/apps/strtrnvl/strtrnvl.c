// Set return value
// Usage
// > strtrnvl -1
// > echo $?
// -1
// >

#include "stdio.h"
#include "stdlib.h"

int main(int argc, char const * const * const argv)
{
	if(2 <= argc)return atoi(argv[1]);
	else
	{
		printf("Usage : > strtrnvl <return value>\n");
		return -1;
	}
}

