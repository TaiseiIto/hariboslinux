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
	popl	%edx	# Discard gs
	popl	%edx	# Discard fs
	popl	%ds
	popl	%edx	# Discard es
	# Initialize registers
	popl	%edx	# Discard eflags
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
	movl	%eax,	%ebx
	movl	$0x00000001,%eax
	int	$0x80
	leave
	ret

