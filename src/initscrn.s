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
	xorw	%ax,	%ax	# disable all serial port interrupts
	movw	$com1,	%dx
	addw	$0x0001,%dx
	outw	%ax,	%dx
	movw	$0x0080,%ax	# enable DLAB
	movw	$com1,	%dx
	addw	$0x0003,%dx
	outw	%ax,	%dx
	movw	$0x0001,%ax	# low byte of baudrate
	movw	$com1,	%dx
	outw	%ax,	%dx
	xorw	%ax	,%ax	# high byte of baudrate
	movw	$com1,	%dx
	addw	$0x0001,%dx
	outw	%ax,	%dx
	movw	$0x0003,%ax	# 8bit per character
	movw	$com1,	%dx
	addw	$0x0003,%dx
	outw	%ax,	%dx
	movw	$0x0041,%ax	# send 'A'
	movw	$com1,	%dx
	outw	%ax,	%dx
2:				# init screen
	movw	$0x0013,%ax	# VGA 320*200*8bit color
	int	$0x10
3:				# free stack frame
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
4:				# halt loop
	hlt
	jmp	4b

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

	.data
hello_message:
	.string	"Hello, initscrn.bin!\n"

