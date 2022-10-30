# This program is OS entry point loaded and called by BIOS
# This program loads a part of the floppy disk (cylinder 0x0000, head 0x0001, sector 0x0001~0x0012) and jumps to loaddisk.bin

# bibliography
# https://wiki.osdev.org/FAT

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.globl	main
	.globl	new_line
	.globl	print
	.globl	print_byte_hex
	.globl	putchar
	.globl	read_sector

	.type	main,		@function
	.type	new_line,	@function
	.type	print,		@function
	.type	print_byte_hex,	@function
	.type	putchar,	@function
	.type	read_sector,	@function

	.code16					# real mode
	.text
stack:						# bottom of stack
entry:
0:
	jmp	main
	nop
	.ascii	"HARIBOTE"	# OEM identifier
	.word	sector_size	# The number of bytes per sector
	.byte	cluster_size	# Number of sectors per cluster
	.word	boot_clusters	# Number of reserved sectors. The boot record sectors are included in this value. After the sectors, FAT sectors begin.
	.byte	fats		# Number of FATs
	.word	files		# Number of root directory entries
	.word	sectors		# Number of sectors in the logical volume
	.byte	media_type	# Media dexcriptor type
	.word	fat_size	# Number of sectors per FAT
	.word	track_size	# Number of sectors per track
	.word	heads		# Number of heads or sides on the storage media
	.long	hidden_sectors	# Number of hidden sectors
	.long	large_sectors	# Number of sectors in the logical volume
	.byte	drive_number	# Drive number
	.byte	0x00		# Reserved
	.byte	boot_signature	# boot sector signature
	.long	volume_serial	# volume serial number
	.ascii	"HARIBOTEOS "	# volume label
	.ascii	"FAT12   "	# FAT file system type

main:
0:				# init registers except CS
	xorw	%ax,	%ax
	movw	%ax,	%bx
	movw	%ax,	%cx
	movw	%ax,	%dx
	movw	%ax,	%si
	movw	%ax,	%di
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax, 	%es
	movw	%ax, 	%fs
	movw	%ax, 	%gs
	movw	$stack,	%bp
	movw	$stack,	%sp
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x000c,%sp
	movw	%sp,	%di
1:				# print hello_message
	movw	$hello_message,(%di)
	call	print
2:				# read the floppy disk (0x2400~0x47ff) to memory (0xa000~0xc400)
	movw	$0x0000,(%di)# cylinder_number
	movw	$0x0001,0x02(%di)# head
	movw	$0x0001,0x04(%di)# sector_number
	movw	$track_size,0x06(%di)# num_of_sectors
	movw	$entry,	%dx
	shrw	$0x0004,%dx
	movw	%dx,	0x08(%di)# destination_segment
	movw	$0x2400,0x0a(%di)	# destination_address
	call	read_sector
	cmpw	$0x0000,%ax
	je	4f
3:				# read_sector failure
	movw	$error_message,(%di)
	call print
4:				# check loaddisk.bin
	call	new_line
	movw	$check_file_message,(%di)
	call	print
5:				# print loaddisk.bin (floppy 0x4200~0x420f, memory 0xbe00~0xbe0f)
	movw	$entry,	%si
	addw	$load_disk,%si
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
				# print new line
	call	new_line
	addw	$0x000c,%sp
	popw	%di
	popw	%si
	leave
8:				# jump to loaddisk.bin
	movw	$entry,	%dx
	addw	$load_disk,%dx
	jmp	*%dx

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
	movzxb	(%si),	%ax
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
print_byte_hex:			# void print_byte_hex(unsigned char value);
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

				# // print a character to console
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
				# cylinder_number: 0x04(%bp)
				# head: 0x06(%bp)
				# sector_number: 0x08(%bp)
				# num_of_sectors: 0x0a(%bp)
				# destination_segment: 0x0c(%bp)
				# destination_address: 0x0e(%bp)
	movw	$0x10,	%cx	# number of trials
1:
	movw	%cx,	0x02(%di)
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

	.data
check_file_message:
	.string "The first 0x10 bytes of loaddisk.bin\r\n"
error_message:
	.string "ERROR!\r\n"
hello_message:
	.string	"Hello, World!\r\n"

