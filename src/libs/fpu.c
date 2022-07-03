#include "fpu.h"
#include "io.h"

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

