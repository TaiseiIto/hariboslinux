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
	float floating_point[2];
	double double_floating_point;
	void *general_pointer;
	char *string;
	unsigned int *unsigned_int_pointer;
} VariadicArg;

// FPU instructions
void f2xm1(void);
void fcos(void);
void fldcw(unsigned short *control);
void fldl(double *x);
void fldl2e(void);
void fldpi(void);
void fnstcw(unsigned short *control);
void fpatan(void);
void fptan(void);
void frndint(void);
void fsin(void);
void fsqrt(void);
void fstpl(double *x);
void fyl2x(void);

// get nth arg in caller variadic arg function
// the first arg is 0th
unsigned int get_caller_variadic_arg(unsigned int);

void const *get_esp(void);

unsigned int system_call(unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi, unsigned int ebp);

#endif

