#include "fpu.h"
#include "io.h"

double fpu_atan(double x)
{
	fldl(&x);
	fld1();
	fpatan();
	fstpl(&x);
	return x;
}

double fpu_cos(double x)
{
	fldl(&x);
	fcos();
	fstpl(&x);
	return x;
}

double fpu_floor(double x)
{
	unsigned short original_control;
	unsigned short changed_control;
	fnstcw(&original_control);
	changed_control = (original_control & ~(FPU_CONTROL_ROUND_TOWARD_NEAREST | FPU_CONTROL_ROUND_DOWN | FPU_CONTROL_ROUND_UP | FPU_CONTROL_ROUND_TOWARD_ZERO)) | FPU_CONTROL_ROUND_DOWN;
	fldcw(&changed_control);
	fldl(&x);
	frndint();
	fstpl(&x);
	fldcw(&original_control);
	return x;
}

double fpu_get_pi(void)
{
	double pi;
	fldpi();
	fstpl(&pi);
	return pi;
}

double fpu_sin(double x)
{
	fldl(&x);
	fsin();
	fstpl(&x);
	return x;
}

double fpu_sqrt(double x)
{
	fldl(&x);
	fsqrt();
	fstpl(&x);
	return x;
}

double fpu_tan(double x)
{
	fldl(&x);
	fptan();
	fincstp();
	fstpl(&x);
	return x;
}

