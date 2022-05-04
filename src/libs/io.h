#ifndef _IO_H_
#define _IO_H_

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

// get nth arg in caller variadic arg function
// the first arg is 0th
unsigned int get_caller_variadic_arg(unsigned int);

void const *get_esp(void);

unsigned int system_call(unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi, unsigned int ebp);

#endif

