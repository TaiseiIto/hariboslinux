#include "fpu.h"
#include "io.h"

double fpu_atan(double x)
{
	double result;
	fldl(&x);
	fld1();
	fpatan();
	fstpl(&result);
	return result;
}

double fpu_cos(double x)
{
	double result;
	fldl(&x);
	fcos();
	fstpl(&result);
	return result;
}

double fpu_e(void)
{
	return fpu_power(2.0, fpu_log2e());
}

double fpu_floor(double x)
{
	double result;
	unsigned short original_control;
	unsigned short changed_control;
	fnstcw(&original_control);
	changed_control = (original_control & ~(FPU_CONTROL_ROUND_TOWARD_NEAREST | FPU_CONTROL_ROUND_DOWN | FPU_CONTROL_ROUND_UP | FPU_CONTROL_ROUND_TOWARD_ZERO)) | FPU_CONTROL_ROUND_DOWN;
	fldcw(&changed_control);
	fldl(&x);
	frndint();
	fstpl(&result);
	fldcw(&original_control);
	return result;
}

double fpu_log(double base, double power)
{
	return fpu_log2(power) / fpu_log2(base);
}

double fpu_log2(double power)
{
	double result;
	fld1();
	fldl(&power);
	fyl2x();
	fstpl(&result);
	return result;
}

double fpu_log2e(void)
{
	double result;
	fldl2e();
	fstpl(&result);
	return result;
}

double fpu_pi(void)
{
	double pi;
	fldpi();
	fstpl(&pi);
	return pi;
}

double fpu_power(double base, double exponent)
{
	double result;
	if(base == 0.0)return 0.0;
	else if(0.0 < base)
	{
		fldl(&exponent);
		fldl(&base);
		fyl2x();
		f2xm1();
		fstpl(&result);
		return result + 1.0;
	}
	else // base < 0.0
	{
		// Return NaN
		return 0.0 / 0.0;
	}
}

double fpu_sin(double x)
{
	double result;
	fldl(&x);
	fsin();
	fstpl(&result);
	return result;
}

double fpu_sqrt(double x)
{
	double result;
	fldl(&x);
	fsqrt();
	fstpl(&result);
	return result;
}

double fpu_tan(double x)
{
	double result;
	fldl(&x);
	fptan();
	fincstp();
	fstpl(&result);
	return result;
}
