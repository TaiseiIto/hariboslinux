# This program is called from bootsector.s to load the floppy disk
#
# Source disk address 0x0:0000~0xf:83ff
# Destination memory address 0x0:7c00~0xf:ffff
#
# Source disk address 0x0:0000~0x0:01ff is already loaded into destination memory address 0x0:7c00~0x0:7dff by BIOS
# Source disk address 0x0:2400~0x0:47ff is already loaded into destination memory address 0x0:a000~0x0:c3ff by bootsector.bin
#
# So this program loads source disk address 0x0:0200~0x0:23ff into destination memory address 0x0:7e00~0x0:9fff
#                   and source disk address 0x0:4800~0xf:83ff into destination memory address 0x0:c400~0xf:ffff
#
# disk address 0x0:0200~0x0:23ff
#	from cylinder 0x0000, head 0x0000, sector 0x0002
#	to   cylinder 0x0000, head 0x0000, sector 0x0012
#
# disk address 0x0:4800~0xf:83ff
#	from cylinder 0x0001, head 0x0000, sector 0x0001
#	to   cylinder 0x0037, head 0x0000, sector 0x0006

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.code16				# real mode
	.set	load_dest,	0x7c00	# memory address the disk loaded to
	.text
entry:
0:
	jmp	main

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
	subw	$0x0006,%sp
	movw	%sp,	%di
				# cylinder_number: 0x0e(%bp)
				# head: 0x0c(%bp)
				# sector_number: 0x0a(%bp)
				# num_of_sectors: 0x08(%bp)
				# destination_segment: 0x06(%bp)
				# destination_address: 0x04(%bp)
	movw	$0x10,	%cx	# number of trials
1:
	movw	%cx,	0x04(%di)
	movb	$0x02,	%ah	# read sectors
	movb	0x08(%bp),%al	# number of read sectors
	movw	0x0e(%bp),%cx	# cylinder_number
	rolw	$0x08,	%cx
	shlb	$0x06,	%cl
	addb	0x0a(%bp),%cl	# sector_number
	xorb	%dl,	%dl	# read from A drive
	movb	0x0c(%bp),%dh	# head
	movw	0x06(%bp),%es	# destination_segment
	movw	0x04(%bp),%bx	# destination_address
	int	$0x13
	jc	3f
2:				# success
	xorw	%ax,	%ax
	jmp	4f
3:				# failure
	shrw	$0x0008,%ax
	movw	%ax,	0x02(%di)
	movw	$int13_error_message,(%di)
	call	print
	movw	0x02(%di),%ax
	movw	%ax,	(%di)
	call	print_byte_hex
	call	new_line
	movw	0x04(%di),%cx
	decw	%cx
	jnz	1b		# retry
	movw	$error_message,(%di)
	call	print
	movw	$0x0001,%ax
4:
	addw	$0x0006,%sp
	popw	%es
	popw	%di
	popw	%bx
	leave
	ret

main:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x000c,%sp
	movw	%sp,	%di
1:				# print hello_message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:				# load disk
				#  from cylinder 0x0000, head 0x0000, sector 0x0002
				#  to   cylinder 0x0000, head 0x0000, sector 0x0012
				# source disk        address 0x0:0200~0x0:23ff
				# destination memory address 0x0:7e00~0x0:9fff
	movw	$0x0000,0x0a(%di)# cylinder_number
	movw	$0x0000,0x08(%di)# head
	movw	$0x0002,0x06(%di)# sector_number
	movw	$0x0011,0x04(%di)# num_of_sectors
	movw	$load_dest,%dx
	shrw	$0x0004,%dx
	movw	%dx,	0x02(%di)# destination_segment
	movw	$0x0200,(%di)	# destination_address
	call	read_sector
	cmpw	$0x0000,%ax
	je	4f
3:				# read_sector failure
	movw	$error_message,(%di)
	call print
4:				# check loaded FAT
	call	new_line
	movw	$check_fat_message,(%di)
	call	print
5:				# print FAT
				# disk address 0x0:0200~0x0:020f
				# memory address 0x0:7e00~0x0:7e0f
	movw	$load_dest, %si
	addw	$0x0200,%si
	movw	$0x0010,%cx
6:				# print each byte loop
	jcxz	7f
	movw	%cx,	0x02(%di)
	movw	(%si),	%dx
	movw	%dx,	(%di)
	call	print_byte_hex
	movw	$0x0020,(%di)	# print space
	call	putchar
	movw	0x02(%di),%cx
	incw	%si
	decw	%cx
	jmp	6b
7:				# end of print each byte loop
	call	new_line
8:				# load disk
				#  from cylinder 0x0001, head 0x0000, sector 0x0001
				#  to   cylinder 0x0036, head 0x0001, sector 0x0012
				# source disk        address 0x0:4800~0xf:77ff
				# destination memory address 0x0:c400~0xf:f3ff
	movw	$0x0001,0x0a(%di)# cylinder_number
	movw	$0x0000,0x08(%di)# head
	movw	$0x0001,0x06(%di)# sector_number
	movw	$0x0001,0x04(%di)# num_of_sectors
	movw	$0x0c40,0x02(%di)# destination_segment
	movw	$0x0000,(%di)	# destination_address
9:				# load loop
	call	read_sector
	movw	0x02(%di),%cx	# advance destination_segment 1 segment
	addw	$0x0020,%cx
	movw	%cx,	0x02(%di)
	movw	0x06(%di),%cx	# advance sector_number
	cmpw	$0x0012,%cx
	je	10f
	incw	%cx
	movw	%cx,	0x06(%di)
	jmp	9b
10:
	movw	$0x0001,0x06(%di)# reset sector_number
	movw	0x08(%di),%cx	# advance head
	jcxz	11f
	jmp	12f
11:				# advance head
	incw	%cx
	movw	%cx,	0x08(%di)
	jmp	9b
12:				# advance cylinder_number
	decw	%cx		# reset head
	movw	%cx,	0x08(%di)
	movw	0x0a(%di),%cx	# advance cylinder_number
	incw	%cx
	cmp	$0x000a,%cx
	je	13f		# finish loading
	movw	%cx,	0x0a(%di)
	jmp	9b
13:				# load disk
				#  from cylinder 0x0037, head 0x0000, sector 0x0001
				#  to   cylinder 0x0037, head 0x0000, sector 0x0006
				# source disk        address 0xf:7800~0xf:83ff
				# destination memory address 0xf:f3ff~0xf:ffff
	call	new_line
	movw	$finish_loading_message,(%di)
	call	print
	movw	$0xc400,(%di)
	call	print
14:				# free stack frame
	addw	$0x000c,%sp
	popw	%di
	popw	%si
	leave
15:				#halt loop
	hlt
	jmp	15b

	.data
check_fat_message:
	.string "The first 0x10 bytes of FAT\r\n"
error_message:
	.string "ERROR!\r\n"
finish_loading_message:
	.string "finish loading!\r\n"
hello_message:
	.string	"Hello, loaddisk.bin!\r\n"
int13_error_message:
	.string "INT 0x13 ERROR AH = 0x"
