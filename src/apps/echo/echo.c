#include "stdio.h"

int main(unsigned int argc, char **argv)
{
	for(unsigned int argi = 0; argi < argc; argi++)puts(argv[argi]);
	return 0;
}

