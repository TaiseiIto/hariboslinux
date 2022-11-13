# This program is called from kernel.bin to call bios functions.
# This program switch from protected mode to real mode, call bios function, and return to protected mode.

	.globl	call_bios
	.type	call_bios,	@function

	.text
	.code32
call_bios:
0:
	pushl	%ebp
	movl	%esp,	%ebp
1:
	leave
	ret

