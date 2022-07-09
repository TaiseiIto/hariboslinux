# Application Binary Interface: cdecl
# Calling convention: System V i386
# Return values: eax, edx
# Parameters: stack
# Scratch registers: eax, ecx, edx
# Preserved registers: ebx, esi, edi, ebp, esp

	.globl	f2xm1
	.globl	fcos
	.globl	fldcw
	.globl	fldl
	.globl	fldl2e
	.globl	fldpi
	.globl	fnstcw
	.globl	fpatan
	.globl	fptan
	.globl	frndint
	.globl	fsin
	.globl	fsqrt
	.globl	fstpl
	.globl	fyl2x
	.globl	get_caller_variadic_arg
	.globl	get_esp
	.globl	system_call

	.type	f2xm1,				@function
	.type	fcos,				@function
	.type	fldcw,				@function
	.type	fldl,				@function
	.type	fldl2e,				@function
	.type	fldpi,				@function
	.type	fnstcw,				@function
	.type	fpatan,				@function
	.type	fptan,				@function
	.type	frndint,			@function
	.type	fsin,				@function
	.type	fsqrt,				@function
	.type	fstpl,				@function
	.type	fyl2x,				@function
	.type	get_caller_variadic_arg,	@function
	.type	get_esp,			@function
	.type	system_call,			@function

	.text

f2xm1:				# void f2xm1(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	f2xm1
	leave
	ret

fcos:				# void fcos(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fcos
	leave
	ret

fldcw:				# void fldcw(unsigned short *control);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	fldcw	(%edx)
	leave
	ret

fldl:				# void fldl(double *x);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	fldl	(%edx)
	leave
	ret

fldl2e:				# void fldl2e(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fldl2e
	leave
	ret

fldpi:				# void fldpi(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fldpi
	leave
	ret

fnstcw:				# void fnstcw(unsigned short *control);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	fnstcw	(%edx)
	leave
	ret

fpatan:				# void fpatan(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fpatan
	leave
	ret

fptan:				# void fptan(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fptan
	leave
	ret

frndint:			# void frndint(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	frndint
	leave
	ret

fsin:				# void fsin(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fsin
	leave
	ret

fsqrt:				# void fsqrt(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fsqrt
	leave
	ret

fstpl:				# void fstpl(double *x);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	fstpl	(%edx)
	leave
	ret

fyl2x:				# void fyl2x(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fyl2x
	leave
	ret

				# // get nth arg in caller variadic arg function
				# // the first arg is 0th
get_caller_variadic_arg:	# unsigned int get_caller_variadic_arg(unsigned int);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	movl	(%ebp),	%esi
	movl	(%esi),	%esi
	movl	0x08(%ebp),%edx
	movl	0x08(%esi,%edx,0x04),%eax
	popl	%esi
	leave
	ret

get_esp:			# void const *get_esp(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	%esp,	%eax
	leave
	ret

# unsigned int system_call
# (
# 	unsigned int eax /* 0x08(%ebp) */ ,
# 	unsigned int ebx /* 0x0c(%ebp) */ ,
# 	unsigned int ecx /* 0x10(%ebp) */ ,
# 	unsigned int edx /* 0x14(%ebp) */ ,
# 	unsigned int esi /* 0x18(%ebp) */ ,
# 	unsigned int edi /* 0x1c(%ebp) */ ,
# 	unsigned int ebp /* 0x20(%ebp) */ 
# );
system_call:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp # Space for return value
	pushal
	movl	0x08(%ebp),%eax
	movl	0x0c(%ebp),%ebx
	movl	0x10(%ebp),%ecx
	movl	0x14(%ebp),%edx
	movl	0x18(%ebp),%esi
	movl	0x1c(%ebp),%edi
	movl	0x20(%ebp),%ebp
	int	$0x80
	movl	%eax,	0x20(%esp)
	popal
	popl	%eax
	leave
	ret

