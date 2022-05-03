# Application Binary Interface: cdecl
# Calling convention: System V i386
# Return values: eax, edx
# Parameters: stack
# Scratch registers: eax, ecx, edx
# Preserved registers: ebx, esi, edi, ebp, esp

	.globl	system_call

	.type	system_call,	@function

	.text

# unsigned int system_call
# (
# 	unsigned int eax /* 0x08(%ebp) */ ,
# 	unsigned int ebx /* 0x0c(%ebp) */ ,
# 	unsigned int ecx /* 0x10(%ebp) */ ,
# 	unsigned int edx /* 0x14(%ebp) */ ,
# 	unsigned int esi /* 0x18(%ebp) */ ,
# 	unsigned int edi /* 0x1c(%ebp) */ ,
# 	unsigned int ebp /* 0x20(%ebp) */ 
# );
system_call:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushal
	movl	0x08(%ebp),%eax
	movl	0x08(%ebp),%ebx
	movl	0x08(%ebp),%ecx
	movl	0x08(%ebp),%edx
	movl	0x08(%ebp),%esi
	movl	0x08(%ebp),%edi
	movl	0x08(%ebp),%ebp
	int	$0x80
	popal
	leave
	ret

