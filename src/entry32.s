
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
	subl	$0x00000004,%esp
	call	new_line_serial
	movl	$hello_message,(%esp)
	call	print_serial
	addl	$0x00000004,%esp
	leave
0:					# halt loop
	hlt
	jmp	0b

				# // print LF
new_line_serial:		# void new_line_serial(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp
	movl	$0x0000000a,(%esp)
	call	putchar_serial
	addl	$0x00000004,%esp
	leave
	ret
				# // print string to serial port COM1
print_serial:			# void print_serial(char *string);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	subl	$0x00000004,%esp
	movl	0x08(%ebp),%esi
1:				# put loop
	xorl	%eax,	%eax
	movb	(%esi),	%al
	cmpb	$0x00,	%al
	je	2f
	movl	%eax,	(%esp)
	call	putchar_serial
	incl	%esi
	jmp	1b
2:
	addl	$0x00000004,%esp
	popl	%esi
	leave
	ret

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

	.data
hello_message:
	.string "Hello, entry32.bin!\n"
