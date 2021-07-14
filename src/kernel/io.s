	.text
	.globl	io_hlt
	.type	io_hlt,	@function

					# // wait for next interrupt
					# void io_hlt(void);
io_hlt:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	hlt
	leave
	ret

