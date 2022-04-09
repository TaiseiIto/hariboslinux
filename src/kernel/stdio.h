#ifndef _STDIO_H_
#define _STDIO_H_

#include "stdbool.h"
#include "stddef.h"

typedef union
{
	long long int long_long_int;
	unsigned long long int unsigned_long_long_int;
	int ints[2];
	unsigned int unsigned_ints[2];
	short shorts[4];
	unsigned short unsigned_shorts[4];
	char chars[8];
	unsigned char unsigned_chars[8];
	void *general_pointer;
	char *string;
	unsigned int *unsigned_int_pointer;
} VariadicArg;

int sprintf(char *str, const char *format, ...);

#endif

