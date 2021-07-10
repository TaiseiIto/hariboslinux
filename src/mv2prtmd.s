# This program is called from initscrn.s to move to protected mode

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.code16				# real mode
	.set	com1,	0x03f8
	.text
main:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
1:					# print hello message
	call	new_line_serial
	movw	$hello_message,(%di)
	call	print_serial
2:					# prepare to move to protected mode
	movw	$0x00ff,%ax		# disable master PIC
	movw	$0x0021,%dx
	outb	%al,	%dx
	movw	$0x00ff,%ax		# disable slave PIC
	movw	$0x00a1,%dx
	outb	%al,	%dx
	cli				# disable interrupts
	movw	$0x0064,0x02(%di)	# enable memory space beyond 0xf:ffff
	movw	$0x00d1,(%di)
	call	send_byte_to_keyboard
	movw	$0x0060,0x02(%di)
	movw	$0x00df,(%di)
	call	send_byte_to_keyboard
	call	wait_for_keyboard
3:					# free stack frame
	addw	$0x0004,%sp
	popw	%di
	leave
4:					# halt loop
	hlt
	jmp	4b

				# // print LF
new_line_serial:		# void new_line_serial(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000a,(%di)	# print LF
	call	putchar_serial
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print string to serial port COM1
print_serial:			# void print_serial(char *string);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%si
1:				# put loop
	xorb	%ah,	%ah
	movb	(%si),	%al
	cmpb	$0x00,	%al
	je	2f		# finish putting all characters
	movw	%ax,	(%di)
	call	putchar_serial
	incw	%si
	jmp	1b		# put next character
2:				# finish putting all characters
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print a character to console
putchar_serial:			# void putchar_serial(char c);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
1:				# wait for device
	movw	$com1,	%dx
	addw	$0x0005,%dx
	inb	%dx,	%al
	andb	$0x20,	%al
	jz	1b
2:				# send the character
	movb	0x04(%bp),%al
	movw	$com1,	%dx
	outb	%al,	%dx
3:				# free stack frame
	popw	%bx
	leave
	ret

send_byte_to_keyboard:		# void send_byte_to_keyboard(unsigned short port, unsigned short data);
0:
	pushw	%bp
	movw	%sp,	%bp
	call	wait_for_keyboard
	movw	0x06(%bp),%dx
	movb	0x04(%bp),%al
	outb	%al,	%dx
	leave
	ret

wait_for_keyboard:		# void wait_for_keyboard(void);
0:
	pushw	%bp
	movw	%sp,	%bp
1:
	movw	$0x0064,%dx
	inb	%dx,	%al
	andb	$0x02,	%al
	jnz	1b
	leave
	ret

	.data
hello_message:
	.string	"Hello, mv2prtmd.bin!\n"
