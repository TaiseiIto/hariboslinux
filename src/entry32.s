
# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.code32				# real mode
	.text
main:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	0x00000041
	call	putchar_serial
	leave
0:					# halt loop
	hlt
	jmp	0b

				# // print a character to console
putchar_serial:			# void putchar_serial(char c);
0:
	pushl	%ebp
	movl	%esp,	%ebp
1:				# wait for device
	movl	$com1,	%edx
	addl	$0x00000005,%edx
	inb	%dx,	%al
	andb	$0x20,	%al
	jz	1b
2:				# send the character
	movb	0x08(%ebp),%al
	movl	$com1,	%edx
	outb	%al,	%dx
3:
	leave
	ret

