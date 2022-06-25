#include "stdio.h"

int main(int argc, char const * const * const argv)
{
	for(int argi = 0; argi < argc; argi++)printf("argv[%d] = %s\n", argi, argv[argi]);
	return 0;
}

