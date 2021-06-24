# bibliography
# https://wiki.osdev.org/FAT

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.code16					# real mode
	.set	sector_size,	0x0200		# The number of bytes per sector
	.set	cluster_size,	0x01		# Number of sectors per cluster
	.set	boot_clusters,	0x0001		# Number of reserved sectors. The boot record sectors are included in this value. After the sectors, FAT sectors begin.
	.set	fats,		0x02		# Number of FATs
	.set	files,		0x00e0		# Number of root directory entries
	.set	sectors,	0x0b40		# Number of sectors in the logical volume
	.set	media_type,	0xf0		# Media dexcriptor type
	.set	fat_size,	0x0009		# Number of sectors per FAT
	.set	track_size,	0x0012		# Number of sectors per track
	.set	heads,		0x0002		# Number of heads or sides on the storage media
	.set	hidden_sectors,	0x00000000	# Number of hidden sectors
	.set	large_sectors,	0x00000b40	# Number of sectors in the logical volume
	.set	drive_number,	0x00		# Drive number
	.set	boot_signature,	0x29		# boot sector signature
	.set	volume_serial,	0xffffffff	# volume serial number

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
1:				# print hello_message
	pushw	$hello_message
	call	print
	addw	$0x0002,%sp
2:				# read the second sector of the bootable floppy disk
	pushw	$0x0000		# cylinder_number
	pushw	$0x0000		# head
	pushw	$0x0002		# sector_number
	movw	$entry,	%dx
	shrw	$0x0004,%dx
	pushw	%dx		# destination_segment
	pushw	$sector_size	# destination_address
	call	read_sector
	addw	$0x000a,%sp
	cmpw	$0x0000,%ax
	je	4f
3:				# read_sector failure
	pushw	$error_message
	call print
	addw	$0x0002,%sp
4:				# print hello_message
	call	new_line
	pushw	$check_FAT_message
	call	print
	addw	$0x0002,%sp
5:				# print bytes from 0x0200($entry) to 0x0210($entry)
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$entry,	%si
	addw	$sector_size,%si
	movw	$0x0010,%cx
6:				# print each byte loop
	cmpw	$0x0000,%cx
	je	7f
	movw	(%si),	%dx
	movw	%dx,	(%di)
	call	print_byte_hex
	movw	$0x0020,(%di)	# print space
	call	putchar
	incw	%si
	decw	%cx
	jmp	6b
7:				# end of print each byte loop
				# print new line
	call	new_line
	leave
8:				# halt loop
	hlt
	jmp	8b

				# // print CRLF
new_line:			# void new_line(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000d,(%di)	# print CR
	call	putchar
	movw	$0x000a,(%di)	# print LF
	call	putchar
	addw	$0x0002,%sp
	leave
	ret

				# // print string to console
print:				# void print(char *string);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
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
	popw	%bx
	leave
	ret

				# // print value as hexadecimal
print_byte_hex:			# void print_byte_hex(unsigned value);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%dx	# print the digit of 0x10s place
	shrw	$0x0004,%dx
	andw	$0x000f,%dx
	cmpw	$0x000a,%dx
	jge	2f
1:				# the digit is less than 0x0a
	addw	$0x0030,%dx
	jmp	3f
2:				# the digit is greater than or equal to 0x0a
	subw	$0x000a,%dx
	addw	$0x0061,%dx
3:				# print
	movw	%dx,	(%di)
	call	putchar
	movw	0x04(%bp),%dx	# print the digit of 0x01s place
	andw	$0x000f,%dx
	cmpw	$0x000a,%dx
	jge	5f
4:				# the digit is less than 0x0a
	addw	$0x0030,%dx
	jmp	6f
5:				# the digit is greater than or equal to 0x0a
	subw	$0x000a,%dx
	addw	$0x0061,%dx
6:				# print
	movw	%dx,	(%di)
	call	putchar
	addw	$0x0002,%sp
	pop	%di
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
				# // destination: [destination_segment:destination_address]
				# // return value 0 means success
				# // return value 1 means failure
read_sector:			# unsigned short read_sector(unsigned short cylinder_number, unsigned short head, unsigned short sector_number, unsigned short destination_segment, unsigned short destination_address);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	pushw	%es
				# cylinder_number: 0x0c(%bp)
				# head: 0x0a(%bp)
				# sector_number: 0x08(%bp)
				# destination_segment: 0x06(%bp)
				# destination_address: 0x04(%bp)
	movb	$0x02,	%ah	# read sectors
	movb	$0x01,	%al	# number of read sectors
	movb	0x0d(%bp),%cl	# cl = (((cylinder_number >> 0x08 ) & 0x03) << 6) | sector_number;
	andb	$0x03,	%cl
	shlb	$0x06,	%cl
	addb	0x08(%bp),%cl
	movb	0x0c(%bp),%ch	# cylinder_number
	xorb	%dl,	%dl	# read from A drive
	movb	0x0a(%bp),%dh	# head
	movw	0x06(%bp),%es	# destination_segment
	movw	0x04(%bp),%bx	# destination_address
	int	$0x13
	jc	2f
1:				# success
	xorw	%ax	,%ax
	jmp	3f
2:				# failure
	pushw	$error_message
	call	print
	movw	$0x0001,%ax
3:
	popw	%es
	popw	%bx
	leave
	ret

	.data
check_FAT_message:
	.string "The first 0x10 bytes of the FAT\r\n"
error_message:
	.string "ERROR!\r\n"
hello_message:
	.string	"Hello, World!\r\n"

