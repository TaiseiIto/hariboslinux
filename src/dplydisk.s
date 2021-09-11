# This program is called from mv2prtmd.s to deploy disk image to 0x00100000 and jump to kernel

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.globl	main
	.globl	putchar_serial

	.type	main,		@function
	.type	putchar_serial,	@function

	.code32
	.text
main:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp
	movl	$0x00000041,%edx
	movl	%edx,	(%esp)
	call	putchar_serial
1:
	addl	$0x00000004,%esp
	leave
2:
	hlt
	jmp	2b

				# // print a character to console
putchar_serial:			# void putchar_serial(char c);
0:
	pushl	%ebp
	movl	%esp,	%ebp
1:				# wait for device
	movw	$com1,	%dx
	addw	$0x0005,%dx
	inb	%dx,	%al
	andb	$0x20,	%al
	jz	1b
2:				# send the character
	movb	0x08(%ebp),%al
	movw	$com1,	%dx
	outb	%al,	%dx
3:
	leave
	ret

	.data
	.align	0x0200
kernel:

