# This program is called from bootsector.s to load the floppy disk
#
# Source disk address 0x0:0000~0x7:83ff
# Destination memory address 0x0:7c00~0x7:ffff
#
# Source disk address 0x0:0000~0x0:01ff is already loaded into destination memory address 0x0:7c00~0x0:7dff by BIOS
# Source disk address 0x0:2400~0x0:47ff is already loaded into destination memory address 0x0:a000~0x0:c3ff by bootsector.bin
#
# So this program loads source disk address 0x0:0200~0x0:23ff into destination memory address 0x0:7e00~0x0:9fff
#                   and source disk address 0x0:4800~0x7:83ff into destination memory address 0x0:c400~0x7:ffff
#
# disk address 0x0:0200~0x0:23ff
#	from cylinder 0x0000, head 0x0000, sector 0x0002
#	to   cylinder 0x0000, head 0x0000, sector 0x0012
#
# disk address 0x0:4800~0xf:81ff
#	from cylinder 0x0001, head 0x0000, sector 0x0001
#	to   cylinder 0x001a, head 0x0001, sector 0x0008

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.code16				# real mode
	.text
main:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x000e,%sp
	movw	%sp,	%di
1:				# print hello_message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:				# check low memory size
	int	$0x12		#  %ax = $0x027f (KB)
	shlw	$0x0006,%ax	#  %ax = $0x9fc0 (the end of the low memory segment)
	movw	%ax,	0x0c(%di)# 0x0c(%di) = $0x9fc0 (end of the low memory segment)
3:				# load disk
				#  from cylinder 0x0000, head 0x0000, sector 0x0002
				#  to   cylinder 0x0000, head 0x0000, sector 0x0012
				# source disk        address 0x0:0200~0x0:23ff
				# destination memory address 0x0:7e00~0x0:9fff
	movw	$0x0000,(%di)# cylinder_number
	movw	$0x0000,0x02(%di)# head
	movw	$0x0002,0x04(%di)# sector_number
	movw	$0x0011,0x06(%di)# num_of_sectors
	movw	$load_dest,%dx
	shrw	$0x0004,%dx
	movw	%dx,	0x08(%di)# destination_segment
	movw	$0x0200,0x0a(%di)# destination_address
	call	read_sector
	cmpw	$0x0000,%ax
	je	5f
4:				# read_sector failure
	movw	$error_message,(%di)
	call print
5:				# check loaded FAT
	call	new_line
	movw	$check_fat_message,(%di)
	call	print
6:				# print FAT
				# disk address 0x0:0200~0x0:020f
				# memory address 0x0:7e00~0x0:7e0f
	movw	$load_dest, %si
	addw	$0x0200,%si
	movw	%si,	(%di)
	movw	$0x0010,0x02(%di)
	call	dump
	call	new_line
7:				# load disk
				#  from cylinder 0x0001, head 0x0000, sector 0x0001
				#  to   cylinder 0x0019, head 0x0001, sector 0x0012
				# source disk        address 0x0:4800~0x9:4fff
				# destination memory address 0x0:c400~0x9:cbff
	movw	$0x0001,(%di)	# cylinder_number
	movw	$0x0000,0x02(%di)# head
	movw	$0x0001,0x04(%di)# sector_number
	movw	$0x0001,0x06(%di)# num_of_sectors
	movw	$0x0c40,0x08(%di)# destination_segment
	movw	$0x0000,0x0a(%di)# destination_address
8:				# load loop
	call	read_sector
	movw	0x08(%di),%cx	# advance destination_segment 1 segment
	addw	$0x0020,%cx
	movw	%cx,	0x08(%di)
	addw	$0x0020,%cx	# compare destination_segment with the end segment of the low memory
	cmpw	0x0c(%di),%cx
	ja	12f
	movw	0x04(%di),%cx	# advance sector_number
	cmpw	$0x0012,%cx
	je	9f
	incw	%cx
	movw	%cx,	0x04(%di)
	jmp	8b
9:
	movw	$0x0001,0x04(%di)# reset sector_number
	movw	0x02(%di),%cx	# advance head
	jcxz	10f
	jmp	11f
10:				# advance head
	incw	%cx
	movw	%cx,	0x02(%di)
	jmp	8b
11:				# advance cylinder_number
	decw	%cx		# reset head
	movw	%cx,	0x02(%di)
	movw	(%di),%cx	# advance cylinder_number
	incw	%cx
	movw	%cx,	(%di)
	jmp	8b
12:				# finish loading
	call	new_line
	movw	$finish_loading_message,(%di)
	call	print
	call	new_line
	movw	$check_cylinder1_message,(%di)
	call	print
13:				# print cylinder 1
				# disk address 0x0:4800~0x0:480f
				# memory address 0x0:c400~0x0:c40f
	movw	$load_dest,%si
	addw	$0x4800,%si
	movw	%si,	(%di)
	movw	$0x0010,0x02(%di)
	call	dump
	call	new_line
14:				# free stack frame
	addw	$0x000e,%sp
	popw	%di
	popw	%si
	leave
15:				# jump to initscrn.bin
	jmp	initscrn

dump:				# void dump(void *address, unsigned short num_of_bytes);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
				# address: 0x04(%bp)
				# num_of_bytes: 0x06(%bp)
	movw	0x04(%bp), %si
	movw	0x06(%bp),%cx
1:				# print each byte loop
	jcxz	2f
	movw	%cx,	0x02(%di)
	movw	(%si),	%dx
	movw	%dx,	(%di)
	call	print_byte_hex
	movw	$0x0020,(%di)	# print space
	call	putchar
	movw	0x02(%di),%cx
	incw	%si
	decw	%cx
	jmp	1b
2:				# end of print each byte loop
	addw	$0x0004,%sp
	popw	%di
	popw	%si
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
	jae	3f
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
				# cylinder_number: 0x04(%bp)
				# head: 0x06(%bp)
				# sector_number: 0x08(%bp)
				# num_of_sectors: 0x0a(%bp)
				# destination_segment: 0x0c(%bp)
				# destination_address: 0x0e(%bp)
	movw	$0x10,	%cx	# number of trials
1:
	movw	%cx,	0x04(%di)
	movb	$0x02,	%ah	# read sectors
	movb	0x0a(%bp),%al	# number of read sectors
	movw	0x04(%bp),%cx	# cylinder_number
	rolw	$0x08,	%cx
	shlb	$0x06,	%cl
	addb	0x08(%bp),%cl	# sector_number
	xorb	%dl,	%dl	# read from A drive
	movb	0x06(%bp),%dh	# head
	movw	0x0c(%bp),%es	# destination_segment
	movw	0x0e(%bp),%bx	# destination_address
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

	.data
check_cylinder1_message:
	.string "The first 0x10 bytes of cylinder 1\n"
check_fat_message:
	.string "The first 0x10 bytes of FAT\n"
error_message:
	.string "ERROR!\n"
finish_loading_message:
	.string "finish loading!\n"
hello_message:
	.string	"Hello, loaddisk.bin!\n"
int13_error_message:
	.string "INT 0x13 ERROR AH = 0x"
	.align	0x0200
initscrn:

