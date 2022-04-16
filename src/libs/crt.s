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
	pushl	%ebp
	movl	%esp,	%ebp
	call	main
	leave
	lret

