#ifndef _STDIO_H_
#define _STDIO_H_

typedef union
{
	int ints[2];
	long long int long_long_int;
	unsigned int unsigned_ints[2];
	unsigned long long int unsigned_long_long_int;
} VariadicArg;

int sprintf(char *str, const char *format, ...);

#endif

