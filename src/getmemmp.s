# This program is called from loaddisk.s to get memory map

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

# bibliography
# https://wiki.osdev.org/Detecting_Memory_(x86)

	.include	"global.s"

	.globl	main

	.type	main,	@function

	.code16		# real mode
	.text
main:
0:
	jmp	initscrn
	
	.data
	.align 0x0200
initscrn:

