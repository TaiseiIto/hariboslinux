#ifndef _FPU_H_
#define _FPU_H_

typedef struct _FPURegisters
{
	unsigned short coltrol;
	#define FPU_CONTROL_EXCEPTION_MASK_INVALID_OPERATION	0x0001
	#define FPU_CONTROL_EXCEPTION_MASK_DENORMALIZED_OPERAND	0x0002
	#define FPU_CONTROL_EXCEPTION_MASK_ZERO_DIVIDE		0x0004
	#define FPU_CONTROL_EXCEPTION_MASK_OVERFLOW		0x0008
	#define FPU_CONTROL_EXCEPTION_MASK_UNDERFLOW		0x0010
	#define FPU_CONTROL_EXCEPTION_MASK_PRECISION		0x0020
	#define FPU_CONTROL_PRECISION_24_BITS			0x0000
	#define FPU_CONTROL_PRECISION_53_BITS			0x0200
	#define FPU_CONTROL_PRECISION_64_BITS			0x0300
	#define FPU_CONTROL_ROUND_TOWARD_NEAREST		0x0000
	#define FPU_CONTROL_ROUND_DOWN				0x0400
	#define FPU_CONTROL_ROUND_UP				0x0800
	#define FPU_CONTROL_ROUND_TOWARD_ZERO			0x0c00
	unsigned short reserved0;
	unsigned short status;
	#define FPU_STATUS_EXCEPTION_INVALID_OPERATION		0x0001
	#define FPU_STATUS_EXCEPTION_DENORMALIZED_OPERAND	0x0002
	#define FPU_STATUS_EXCEPTION_ZERO_DIVIDE		0x0004
	#define FPU_STATUS_EXCEPTION_OVERFLOW			0x0008
	#define FPU_STATUS_EXCEPTION_UNDERFLOW			0x0010
	#define FPU_STATUS_EXCEPTION_PRECISION			0x0020
	#define FPU_STATUS_STACK_FAULT				0x0040
	#define FPU_STATUS_EXCEPTION_SUMMARY			0x0080
	#define FPU_STATUS_C0					0x0100
	#define FPU_STATUS_C1					0x0200
	#define FPU_STATUS_C2					0x0400
	#define FPU_STATUS_TOP					0x3800
	#define FPU_STATUS_C3					0x4000
	#define FPU_STATUS_BUSY					0x8000
	unsigned short reserved1;
	unsigned short tag;
	#define FPU_TAG_REGISTER_0_VALID	0x0000
	#define FPU_TAG_REGISTER_0_ZERO		0x0001
	#define FPU_TAG_REGISTER_0_SPECIAL	0x0002
	#define FPU_TAG_REGISTER_0_EMPTY	0x0003
	#define FPU_TAG_REGISTER_1_VALID	0x0000
	#define FPU_TAG_REGISTER_1_ZERO		0x0004
	#define FPU_TAG_REGISTER_1_SPECIAL	0x0008
	#define FPU_TAG_REGISTER_1_EMPTY	0x000c
	#define FPU_TAG_REGISTER_2_VALID	0x0000
	#define FPU_TAG_REGISTER_2_ZERO		0x0010
	#define FPU_TAG_REGISTER_2_SPECIAL	0x0020
	#define FPU_TAG_REGISTER_2_EMPTY	0x0030
	#define FPU_TAG_REGISTER_3_VALID	0x0000
	#define FPU_TAG_REGISTER_3_ZERO		0x0040
	#define FPU_TAG_REGISTER_3_SPECIAL	0x0080
	#define FPU_TAG_REGISTER_3_EMPTY	0x00c0
	#define FPU_TAG_REGISTER_4_VALID	0x0000
	#define FPU_TAG_REGISTER_4_ZERO		0x0100
	#define FPU_TAG_REGISTER_4_SPECIAL	0x0200
	#define FPU_TAG_REGISTER_4_EMPTY	0x0300
	#define FPU_TAG_REGISTER_5_VALID	0x0000
	#define FPU_TAG_REGISTER_5_ZERO		0x0400
	#define FPU_TAG_REGISTER_5_SPECIAL	0x0800
	#define FPU_TAG_REGISTER_5_EMPTY	0x0c00
	#define FPU_TAG_REGISTER_6_VALID	0x0000
	#define FPU_TAG_REGISTER_6_ZERO		0x1000
	#define FPU_TAG_REGISTER_6_SPECIAL	0x2000
	#define FPU_TAG_REGISTER_6_EMPTY	0x3000
	#define FPU_TAG_REGISTER_7_VALID	0x0000
	#define FPU_TAG_REGISTER_7_ZERO		0x4000
	#define FPU_TAG_REGISTER_7_SPECIAL	0x8000
	#define FPU_TAG_REGISTER_7_EMPTY	0xc000
	unsigned short reserved2;
	unsigned int instruction_pointer_offset;
	unsigned short instruction_pointer_selector;
	unsigned short opcode;
	unsigned int data_pointer_offset;
	unsigned short data_pointer_selector;
	unsigned short reserved3;
	unsigned char data_registers[8][10];
} FPURegisters;

double fpu_2_to_the(double x);
double fpu_acos(double x);
double fpu_acosh(double x);
double fpu_asin(double x);
double fpu_asinh(double x);
double fpu_atan(double x);
double fpu_atan2(double x, double y);
double fpu_atanh(double x);
double fpu_atan2(double y, double x);
double fpu_cos(double x);
double fpu_cosh(double x);
double fpu_e(void);
double fpu_floor(double x);
double fpu_log(double base, double power);
double fpu_log2(double power);
double fpu_log2e(void);
double fpu_pi(void);
double fpu_pow(double base, double exponent);
double fpu_sin(double x);
double fpu_sinh(double x);
double fpu_sqrt(double x);
double fpu_tan(double x);
double fpu_tanh(double x);
double fpu_trunc(double x);

#endif

