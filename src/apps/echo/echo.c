#include "stdio.h"

int main(int argc, char **argv)
{
	for(int argi = 0; argi < argc; argi++)puts(argv[argi]);
	return 0;
}

