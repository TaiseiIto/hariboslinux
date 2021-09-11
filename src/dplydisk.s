# This program is called from mv2prtmd.s to deploy disk image to 0x00100000 and jump to kernel

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.globl	main

	.type	main,	@function

	.text
main:
0:
	hlt
	jmp	0b

	.data
	.align	0x0200
kernel:

