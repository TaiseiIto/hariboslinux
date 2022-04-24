# Application Binary Interface: cdecl
# Calling convention: System V i386
# Return values: eax, edx
# Parameters: stack
# Scratch registers: eax, ecx, edx
# Preserved registers: ebx, esi, edi, ebp, esp

	.extern	main

	.globl	_start

	.type	_start,	@function

	.text

_start:
0:
	# Initialize segments
	popl	%gs
	popl	%fs
	popl	%ds
	popl	%es
	# Initialize registers
	popfl
	popal
	# Push registers
	pushal
	pushfl
	# Push segments
	pushl	%es
	pushl	%ds
	pushl	%fs
	pushl	%gs
	# Call main
	pushl	%ebp
	movl	%esp,	%ebp
	call	main
	# Return to kernel
	movl	$0x00000001,%eax
	xorl	%ebx,	%ebx
	int	$0x80
	leave
	ret

