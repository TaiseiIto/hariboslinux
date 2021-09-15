#include "libgcc.h"

long long int __divdi3(long long int a, long long int b)
{
	long long int result = 0;
	return result;
}

long long int __moddi3(long long int a, long long int b)
{
	long long int result = 0;
	return result;
}

unsigned long long int __udivdi3(unsigned long long int a, unsigned long long int b)
{
	unsigned long long int result = 0;
	while(b <= a)
	{
		unsigned char shift;
		for(shift = 0; b << shift + 1 <= a; shift++);
		a -= b << shift;
		result += 1 << shift;
	}
	return result;
}

unsigned long long int __umoddi3(unsigned long long int a, unsigned long long int b)
{
	while(b <= a)
	{
		unsigned char shift;
		for(shift = 0; b << shift + 1 <= a; shift++);
		a -= b << shift;
	}
	return a;
}

