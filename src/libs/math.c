#include "fpu.h"
#include "math.h"

double acos(double x)
{
	return fpu_acos(x);
}

double asin(double x)
{
	return fpu_asin(x);
}

double atan(double x)
{
	return fpu_atan(x);
}

double atan2(double y, double x)
{
	return fpu_atan2(y, x);
}

double cos(double x)
{
	return fpu_cos(x);
}

double cosh(double x)
{
	return fpu_cosh(x);
}

double exp(double x)
{
	return fpu_pow(M_E, x);
}

double fabs(double x)
{
	if(0.0 <= x)return x;
	else return -x;
}


double floor(double x)
{
	return fpu_floor(x);
}

double fmod(double x, double y)
{
	return x - floor(x / y) * y;
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

double log(double x)
{
	return fpu_log(M_E, x);
}

double log10(double x)
{
	return fpu_log(10.0, x);
}

double pow(double x, double y)
{
	return fpu_pow(x, y);
}

double sin(double x)
{
	return fpu_sin(x);
}

double sinh(double x)
{
	return fpu_sinh(x);
}

double sqrt(double x)
{
	return fpu_sqrt(x);
}

double tan(double x)
{
	return fpu_tan(x);
}

double tanh(double x)
{
	return fpu_tanh(x);
}

