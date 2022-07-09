#ifndef _MATH_H_
#define _MATH_H_

#include "fpu.h"

#define M_1_PI (1.0 / M_PI)
#define M_2_PI (2.0 / M_PI)
#define M_2_SQRTPI (2.0 / fpu_sqrt(M_PI))
#define M_E (fpu_e())
#define M_LN10 (fpu_log(M_E, 10.0))
#define M_LN2 (fpu_log(M_E, 2.0))
#define M_LOG10E (fpu_log(10.0, M_E))
#define M_LOG2E (fpu_log2e())
#define M_PI (fpu_pi())
#define M_PI_2 (M_PI / 2.0)
#define M_PI_4 (M_PI / 4.0)
#define M_SQRT2 (fpu_sqrt(2.0))
#define M_SQRT1_2 (fpu_sqrt(1.0 / 2.0))

double floor(double x);
double fmod(double x, double y);
long long int llabs(long long int a);
long long int llsign(long long int a);

#endif

