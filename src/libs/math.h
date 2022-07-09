#ifndef _MATH_H_
#define _MATH_H_

#include "fpu.h"

#define M_E (fpu_e())
#define M_PI (fpu_pi())

double floor(double x);
double fmod(double x, double y);
long long int llabs(long long int a);
long long int llsign(long long int a);

#endif

