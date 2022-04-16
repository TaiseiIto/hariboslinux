# Calling convention = System V i386
# Return value: eax, edx
# Parameters: stack
# Scratch registers: eax, ecx, edx
# Preserved registers: ebx, esi, edi, ebp, esp

	.globl	lret

	.type	lret,	@function

lret:
0:
	lret

