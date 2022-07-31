global	_start
	.text
_start:
	movq	rax,	0x3c
	xorq	rdi,	rdi
	syscall

