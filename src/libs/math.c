#include "fpu.h"
#include "math.h"
#include "stdio.h"

double floor(double x)
{
	return fpu_floor(x);
}

double fmod(double x, double y)
{
	printf("begin fmod\n");
	double result = x - floor(x / y) * y;
	printf("end fmod\n");
	return result;
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

