#include "math.h"

int abs(int a)
{
	if(0 <= a)return a;
	else return -a;
}

int sign(int a)
{
	return (0 < a) - (a < 0);
}

long long int llabs(long long int a)
{
	if(0 <= a)return a;
	else return -a;
}

long long int llsign(long long int a)
{
	return (0 < a) - (a < 0);
}

