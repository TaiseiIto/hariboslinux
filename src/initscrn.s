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
	pushw	%si
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
1:				# print hello message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:				# init serial port 0x03f8 (COM1)
	call	init_serial_port_com1
	movw	$hello_serial_message,(%di)
	call	print_serial
3:				# free stack frame
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
4:				# init screen
	movw	$0x0013,%ax	# VGA 320*200*8bit color
	int	$0x10
5:				# push screen information and keyboard state
				#
				# 0x7bf6 void *VRAM_addr;
				# 0x7bfa unsigned short screen_width;
				# 0x7bfc unsigned short screen_height;
				# 0x7bfe unsigned char bits_per_pixel;
				# 0x7bff unsigned char keyboard_state;

	movw	$0x0200,%ax	# keyboard_state
	int	$0x0016
	shlw	$0x0008,%ax
	addw	$0x0008,%ax	# 8 bit per pixel
	pushw	%ax
	movw	$0x00c8,%ax	# screen_height
	pushw	%ax
	movw	$0x0140,%ax	# screen_width
	pushw	%ax
	movw	$0x000a,%ax	# VRAM_addr
	pushw	%ax
	xorw	%ax,	%ax
	pushw	%ax
6:				# check VRAM_addr
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	call	new_line_serial
	movw	$vram_addr_message,(%di)
	call	print_serial
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
7:				# halt loop
	hlt
	jmp	7b

init_serial_port_com1:		# void init_serial_port_com1(void)
	pushw	%bp
	movw	%sp,	%bp
	xorb	%al,	%al	# disable all serial port interrupts
	movw	$com1,	%dx
	addw	$0x0001,%dx
	outb	%al,	%dx
	movb	$0x80,	%al	# enable DLAB
	movw	$com1,	%dx
	addw	$0x0003,%dx
	outb	%al,	%dx
	movb	$0x01,	%al	# low byte of baudrate
	movw	$com1,	%dx
	outb	%al,	%dx
	xorb	%al	,%al	# high byte of baudrate
	movw	$com1,	%dx
	addw	$0x0001,%dx
	outb	%al,	%dx
	movb	$0x03,	%al	# 8bit per character
	movw	$com1,	%dx
	addw	$0x0003,%dx
	outb	%al,	%dx
	leave
	ret

				# // print CRLF
new_line:			# void new_line(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000d,(%di)	# print CR
	call	putchar
	movw	$0x000a,(%di)	# print LF
	call	putchar
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

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

				# // print string to console
print:				# void print(char *string);
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
	cmpb	$0x0a,	%al
	je	2f		# print CRLF
	cmpb	$0x00,	%al
	je	4f		# finish putting all characters
	movw	%ax,	(%di)
	call	putchar
	jmp	3f
2:				# print CRLF
	movw	$0x000d,(%di)
	call	putchar
	movw	$0x000a,(%di)
	call	putchar
3:
	incw	%si
	jmp	1b		# put next character
4:				# finish putting all characters
	addw	$0x0002,%sp
	popw	%di
	popw	%si
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

				# print a character to console
putchar:			# void putchar(char c);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	movb	0x04(%bp),%al
	movb	$0x0e,	%ah
	movw	$0x000f,%bx
	int	$0x10
	popw	%bx
	leave
	ret

				# print a character to console
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

	.data
hello_message:
	.string	"Hello, initscrn.bin!\n"
hello_serial_message:
	.string "Hello, serial port COM1!\n"
vram_addr_message:
	.string "VRAM address = 0x"

