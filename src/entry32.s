
# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.code32				# real mode
	.set	com1,	0x03f8
	.text
main:
0:					# halt loop
	hlt
	jmp	0b

