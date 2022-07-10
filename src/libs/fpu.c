#include "fpu.h"
#include "io.h"
#include "stdio.h"

double fpu_acos(double x)
{
	if(x == 0.0)return fpu_pi() / 2.0;
	else if(0.0 < x && x <= 1.0)return fpu_atan(fpu_sqrt((1.0 - x * x) / (x * x)));
	else if(-1.0 <= x && x < 0.0)return fpu_pi() - fpu_atan(fpu_sqrt((1.0 - x * x) / (x * x)));
	else return 0.0 / 0.0; // Return NaN
}

double fpu_asin(double x)
{
	if(x == 1.0)return fpu_pi() / 2.0;
	if(x == -1.0)return -fpu_pi() / 2.0;
	else if(0.0 <= x && x < 1.0)return fpu_atan(fpu_sqrt(x * x / (1.0 - x * x)));
	else if(1.0 < x && x < 0.0)return -fpu_atan(fpu_sqrt(x * x / (1.0 - x * x)));
	else return 0.0 / 0.0; // Return NaN
}

double fpu_atan(double x)
{
	return fpu_atan2(x, 1.0);
}

double fpu_atan2(double y, double x)
{
	double result;
	fldl(&y);
	fldl(&x);
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

double fpu_cosh(double x)
{
	double e = fpu_e();
	return (fpu_pow(e, x) + fpu_pow(e, -x)) / 2.0;
}

double fpu_e(void)
{
	return fpu_pow(2.0, fpu_log2e());
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

double fpu_pow(double base, double exponent)
{
	double result;
	if(base == 0.0)
	{
		result = 0.0;
		printf("base == 0.0\n");
		printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
		return result;
	}
	else if(0.0 < base)
	{
		if(-1.0 < exponent && exponent < 1.0)
		{
			fldl(&exponent);
			fldl(&base);
			fyl2x();
			f2xm1();
			fstpl(&result);
			result += 1.0;
			printf("-1.0 < exponent && exponent < 1.0\n");
			printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
			return result;
		}
		else if(exponent == fpu_trunc(exponent))
		{
			if(exponent == 1.0)
			{
				result = base;
				printf("exponent == 1.0\n");
				printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
				return base;
			}
			else if(exponent == -1.0)
			{
				result = 1.0 / base;
				printf("exponent == -1.0\n");
				printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
				return result;
			}
			else
			{
				int exponent_integer = (int)exponent;
				int exponent_half = exponent_integer / 2;
				int exponent_remainder = exponent_integer % 2;
				double power_half = fpu_pow(base, exponent_half);
				result = power_half * power_half * fpu_pow(base, exponent_remainder);
				printf("exponent == fpu_trunc(exponent)\n");
				printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
				return result;
			}
		}
		else
		{
			double exponent_integer = fpu_trunc(exponent);
			double exponent_decimal = exponent - exponent_integer;
			result = fpu_pow(base, exponent_integer) * fpu_pow(base, exponent_decimal);
			printf("0.0 < base\n");
			printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
			return result;
		}
	}
	else // if base < 0.0 then return NaN
	{
		result = 0.0 / 0.0;
		printf("base < 0.0\n");
		printf("%.6llf^%.6llf=%.6llf\n", base, exponent, result);
		return result;
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

double fpu_sinh(double x)
{
	double e = fpu_e();
	return (fpu_pow(e, x) - fpu_pow(e, -x)) / 2.0;
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

double fpu_tanh(double x)
{
	double e = fpu_e();
	return (fpu_pow(e, x) - fpu_pow(e, -x)) / (fpu_pow(e, x) + fpu_pow(e, -x));
}

double fpu_trunc(double x)
{
	double result;
	unsigned short original_control;
	unsigned short changed_control;
	fnstcw(&original_control);
	changed_control = (original_control & ~(FPU_CONTROL_ROUND_TOWARD_NEAREST | FPU_CONTROL_ROUND_DOWN | FPU_CONTROL_ROUND_UP | FPU_CONTROL_ROUND_TOWARD_ZERO)) | FPU_CONTROL_ROUND_TOWARD_ZERO;
	fldcw(&changed_control);
	fldl(&x);
	frndint();
	fstpl(&result);
	fldcw(&original_control);
	return result;
}

