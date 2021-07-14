	.set	com1,	0x03f8			# serial port COM1

	.text
	.globl	io_hlt
	.globl	new_line_serial
	.globl	print_serial
	.globl	putchar_serial
	.type	io_hlt,			@function
	.type	new_line_serial,	@function
	.type	print_serial,		@function
	.type	putchar_serial,		@function

				# // wait for next interrupt
io_hlt:				# void __attribute__((stdcall)) io_hlt(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	hlt
	leave
	ret

				# // print LF
new_line_serial:		# void __attribute__((stdcall)) new_line_serial(void);
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
print_serial:			# void __attribute__((stdcall)) print_serial(char *string);
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

				# // print a character to serial port COM1
putchar_serial:			# void __attribute__((stdcall)) putchar_serial(char c);
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


