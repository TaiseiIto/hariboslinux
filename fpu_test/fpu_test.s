	.globl	main
	.text
main:
	movq	$0x000000000000003c,	%rax
	xorq	%rdi,	%rdi
	syscall

