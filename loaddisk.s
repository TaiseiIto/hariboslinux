	.code16			# real mode
	.text
entry:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
1:				# print hello_message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:				# free stack frame
	addw	$0x0002,%sp
	popw	%di
	leave
3:				#halt loop
	hlt
	jmp	3b

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
	cmpb	$0x00,	%al
	je	2f		# finish putting all characters
	movw	%ax,	(%di)
	call	putchar
	incw	%si
	jmp	1b		# put next character
2:
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print value as hexadecimal
print_byte_hex:			# void print_byte_hex(unsigned value);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
	movw	$0x0001,%cx	# if %cx == 1, then print the digit of 0x10s place, else print the digit of 0x01s place.
	movw	0x04(%bp),%dx	# get the byte
	shrw	$0x0004,%dx
1:
	andw	$0x000f,%dx
	cmpw	$0x000a,%dx
	jge	3f
2:				# the digit is less than 0x0a
	addw	$0x0030,%dx
	jmp	4f
3:				# the digit is greater than or equal to 0x0a
	subw	$0x000a,%dx
	addw	$0x0061,%dx
4:				# print the digit
	movw	%cx,	0x02(%di)
	movw	%dx,	(%di)
	call	putchar
	movw	0x02(%di),%cx
	jcxz	5f
	movw	0x04(%bp),%dx	# get the byte
	decw	%cx
	jmp	1b
5:				# finish printing
	addw	$0x0004,%sp
	popw	%di
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

				# // read a sector from A drive
				# // cylinder_number: 0x0000~0x004f
				# // head: 0x0000, 0x0001
				# // sector_number: 0x0000~0x0012
				# // num_of_sectors: 0x0001~0x0012
				# // destination: [destination_segment:destination_address]
				# // return value 0 means success
				# // return value 1 means failure
read_sector:			# unsigned short read_sector(unsigned short cylinder_number, unsigned short head, unsigned short sector_number, unsigned short num_of_sectors, unsigned short destination_segment, unsigned short destination_address);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	pushw	%di
	pushw	%es
	subw	$0x0004,%sp
	movw	%sp,	%di
				# cylinder_number: 0x0e(%bp)
				# head: 0x0c(%bp)
				# sector_number: 0x0a(%bp)
				# num_of_sectors: 0x08(%bp)
				# destination_segment: 0x06(%bp)
				# destination_address: 0x04(%bp)
	movw	$0x10,	%cx	# number of trials
1:
	movw	%cx,	0x02(%di)
	movb	$0x02,	%ah	# read sectors
	movb	0x08(%bp),%al	# number of read sectors
	movb	0x0f(%bp),%cl	# cl = (((cylinder_number >> 0x08 ) & 0x03) << 6) | sector_number;
	andb	$0x03,	%cl
	shlb	$0x06,	%cl
	addb	0x0a(%bp),%cl
	movb	0x0e(%bp),%ch	# cylinder_number
	xorb	%dl,	%dl	# read from A drive
	movb	0x0c(%bp),%dh	# head
	movw	0x06(%bp),%es	# destination_segment
	movw	0x04(%bp),%bx	# destination_address
	int	$0x13
	jc	3f
2:				# success
	xorw	%ax	,%ax
	jmp	4f
3:				# failure
	movw	0x02(%di),%cx
	decw	%cx
	jnz	1b		# retry
	movw	$error_message,(%di)
	call	print
	movw	$0x0001,%ax
4:
	addw	$0x0004,%sp
	popw	%es
	popw	%di
	popw	%bx
	leave
	ret

error_message:
	.string "ERROR!\r\n"
hello_message:
	.string	"Hello, loaddisk.bin!\r\n"
