#include "libgcc.h"
#include "math.h"

long long int __divdi3(long long int a, long long int b)
{
	long long int result = 0;
	while(!(0 <= a && a < llabs(b)))
	{
		unsigned char shift;
		if(0 <= a)for(shift = 0; llsign(a) * llsign(b) * b << shift + 1 <= a; shift++);
		else for(shift = 0; a < llsign(a) * llsign(b) * b << shift; shift++);
		a -= llsign(a) * llsign(b) * b << shift;
		result += llsign(a) * llsign(b) * 1 << shift;
	}
	return result;
}

long long int __moddi3(long long int a, long long int b)
{
	while(!(0 <= a && a < llabs(b)))
	{
		unsigned char shift;
		if(0 <= a)for(shift = 0; llsign(a) * llsign(b) * b << shift + 1 <= a; shift++);
		else for(shift = 0; a < llsign(a) * llsign(b) * b << shift; shift++);
		a -= llsign(a) * llsign(b) * b << shift;
	}
	return a;
}

unsigned long long int __udivdi3(unsigned long long int a, unsigned long long int b)
{
	unsigned long long int result = 0;
	while(!(a < b))
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
	while(!(a < b))
	{
		unsigned char shift;
		for(shift = 0; b << shift + 1 <= a; shift++);
		a -= b << shift;
	}
	return a;
}

