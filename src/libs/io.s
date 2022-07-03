# Application Binary Interface: cdecl
# Calling convention: System V i386
# Return values: eax, edx
# Parameters: stack
# Scratch registers: eax, ecx, edx
# Preserved registers: ebx, esi, edi, ebp, esp

	.globl	fldcw
	.globl	fldl
	.globl	fnstcw
	.globl	frndint
	.globl	fstpl
	.globl	get_caller_variadic_arg
	.globl	get_esp
	.globl	system_call

	.type	fldcw,				@function
	.type	fldl,				@function
	.type	fnstcw,				@function
	.type	frndint,			@function
	.type	fstpl,				@function
	.type	get_caller_variadic_arg,	@function
	.type	get_esp,			@function
	.type	system_call,			@function

	.text

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

fnstcw:				# void fnstcw(unsigned short *control);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	fnstcw	(%edx)
	leave
	ret

frndint:			# void frndint(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	frndint
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

