# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.globl	memcpy
	.globl	memset
	.globl	strchr
	.globl	strcmp
	.globl	strcpy
	.globl	strlen

	.type	memcpy,	@function
	.type	memset,	@function
	.type	strchr,	@function
	.type	strcmp,	@function
	.type	strcpy,	@function
	.type	strlen,	@function

memcpy:				# void *memcpy(void *destination, void const *source, size_t size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	leave
	ret

memset:				# void *memset(void *buf, char ch, size_t size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	leave
	ret

strchr:				# char const *strchr(char const *string, char character);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	leave
	ret

strcmp:				# int strcmp(char const *string1, char const *string2);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	leave
	ret

strcpy:				# char *strcpy(char *destination, char const *source);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	leave
	ret

strlen:				# unsigned int strlen(char const *string);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	leave
	ret

