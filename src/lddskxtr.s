# This program is called from dplydisk.s to load the entire floppy disk switching back and forth between 16bit mode and 32bit mode.
#
# Source disk address        0x00000000~0x00167fff
# Destination memory address 0x00100000~0x00267fff
# Load buffer                0x00007c00~0x0000dc00
#
# calling convention = System V i386
# return value: eax, edx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp
#
# typedef struct
# {
# 	unsigned short cylinder;
# 	unsigned short head;
# 	unsigned short sector;
# } SectorSpecifier;

	.include	"global.s"

	# 32bit mode functions
	.globl	main
	.globl	disk_address_2_sector_specifier
	.globl	load_sectors_32
	.globl	return_2_32
	.globl	memcpy
	.globl	new_line_serial
	.globl	print_byte_hex_serial
	.globl	print_dword_hex_serial
	.globl	print_word_hex_serial
	.globl	print_sector_specifier
	.globl	print_serial
	.globl	putchar_serial
	.globl	sector_specifier_2_disk_address
	.globl	validate_sector_specifier
	# 16bit mode functions
	.globl	load_sectors_16
	.globl	load_sectors
	.globl	load_sector
	.globl	new_line_serial_16
	.globl	print_byte_hex_serial_16
	.globl	print_dword_hex_serial_16
	.globl	print_word_hex_serial_16
	.globl	print_sector_specifier_16
	.globl	print_serial_16
	.globl	putchar_serial_16
	.globl	validate_sector_specifier_16

	# 32bit mode functions
	.type	main,				@function
	.type	disk_address_2_sector_specifier,@function
	.type	load_sectors_32,		@function
	.type	return_2_32,			@function
	.type	memcpy,				@function
	.type	new_line_serial,		@function
	.type	print_byte_hex_serial,		@function
	.type	print_dword_hex_serial,		@function
	.type	print_word_hex_serial,		@function
	.type	print_sector_specifier,		@function
	.type	print_serial,			@function
	.type	putchar_serial,			@function
	.type	sector_specifier_2_disk_address,@function
	.type	validate_sector_specifier,	@function
	# 16bit mode functions
	.type	load_sectors_16,		@function
	.type	load_sectors,			@function
	.type	load_sector,			@function
	.type	new_line_serial_16,		@function
	.type	print_byte_hex_serial_16,	@function
	.type	print_dword_hex_serial_16,	@function
	.type	print_word_hex_serial_16,	@function
	.type	print_sector_specifier_16,	@function
	.type	print_serial_16,		@function
	.type	putchar_serial_16,		@function
	.type	validate_sector_specifier_16,	@function

	.text
	.code32
main:
0:
	lgdt	(gdtr)
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x0000000c,%esp
1:	# print test message
	call	new_line_serial
	movl	$hello_message,(%esp)
	call	print_serial
2:	# print last sector
	movl	$last_sector_message,(%esp)
	call	print_serial
	call	new_line_serial
	movl	$last_cylinder,(%esp)
	call	print_sector_specifier
	movl	$last_disk_address_message,(%esp)
	call	print_serial
	movl	(last_disk_address),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
3:	# calculate buffer range
	movw	(highest_memory_segment),%dx
	movw	%dx,	(buffer_end_segment)
	subw	(buffer_begin_segment),%dx
	movw	%dx,	(buffer_size_segment)
	movzxw	(buffer_begin_segment),%edx
	shll	$0x04,	%edx
	movl	%edx,	(buffer_begin_address)
	movzxw	(buffer_end_segment),%edx
	shll	$0x04,	%edx
	movl	%edx,	(buffer_end_address)
	movzxw	(buffer_size_segment),%edx
	shll	$0x04,	%edx
	movl	%edx,	(buffer_size_address)
	# print buffer_begin_segment
	movl	$buffer_begin_segment_message,(%esp)
	call	print_serial
	movl	(buffer_begin_segment),%edx
	movl	%edx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print buffer_end_segment
	movl	$buffer_end_segment_message,(%esp)
	call	print_serial
	movl	(buffer_end_segment),%edx
	movl	%edx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print buffer_size_segment
	movl	$buffer_size_segment_message,(%esp)
	call	print_serial
	movl	(buffer_size_segment),%edx
	movl	%edx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print buffer_begin_address
	movl	$buffer_begin_address_message,(%esp)
	call	print_serial
	movl	(buffer_begin_address),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	# print buffer_end_address
	movl	$buffer_end_address_message,(%esp)
	call	print_serial
	movl	(buffer_end_address),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	# print buffer_size_address
	movl	$buffer_size_address_message,(%esp)
	call	print_serial
	movl	(buffer_size_address),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
4:	# calculate the first disk range to read
	# get last loaded cylinder
	movzxb	(last_loaded_cylinder),	%dx
	movw	%dx,	(begin_cylinder)
	# get last loaded head
	movzxb	(last_loaded_head),%dx
	movw	%dx,	(begin_head)
	# get last loaded sector
	movzxb	(last_loaded_sector),%dx
	incw	%dx	# calculate next sector
	movw	%dx,	(begin_sector)
	# validate the begin sector specifier
	movl	$begin_cylinder,(%esp)
	call	validate_sector_specifier
5:	# print begin sector
	movl	$begin_sector_message,(%esp)
	call	print_serial
	call	new_line_serial
	movl	$begin_cylinder,(%esp)
	call	print_sector_specifier
6:	# calculate begin_disk_address
	movl	$begin_cylinder,(%esp)
	call	sector_specifier_2_disk_address
	movl	%eax,	(begin_disk_address)
	# check begin_disk_address
	movl	$begin_disk_address_message,(%esp)
	call	print_serial
	movl	(begin_disk_address),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
7:	# calculate end_disk_address
	movl	(begin_disk_address),%eax
	movl	(buffer_size_address),%edx
	addl	%edx,	%eax
	movl	(last_disk_address),%edx
	cmpl	%edx,	%eax
	jb	8f
	movl	%edx,	%eax
8:
	movl	%eax,	(end_disk_address)
	movl	%eax,	(%esp)
	movl	$end_cylinder,0x04(%esp)
	call	disk_address_2_sector_specifier
	# print end_disk_address
	movl	$end_sector_message,(%esp)
	call	print_serial
	call	new_line_serial
	movl	$end_cylinder,(%esp)
	call	print_sector_specifier
	movl	$end_disk_address_message,(%esp)
	call	print_serial
	movl	(end_disk_address),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
9:	# calculate copy destination
	movl	(destination),%eax
	movl	(begin_disk_address),%ebx
	movl	(end_disk_address),%ecx
	movl	%eax,	%edx
	addl	%ecx,	%edx
	addl	%ebx,	%eax
	subl	%ebx,	%ecx
	movl	%eax,	(copy_destination_begin)
	movl	%edx,	(copy_destination_end)
	movl	%ecx,	(copy_size)
	# print copy destination
	movl	$copy_destination_begin_message,(%esp)
	call	print_serial
	movl	(copy_destination_begin),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	movl	$copy_destination_end_message,(%esp)
	call	print_serial
	movl	(copy_destination_end),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	movl	$copy_size_message,(%esp)
	call	print_serial
	movl	(copy_size),%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
10:	# load sectors and copy to destination
	call	load_sectors_32
	# protected mode message
	movl	$protected_mode_message,(%esp)
	call	print_serial
	call	new_line_serial
	call	new_line_serial
	# copy to destination
	movl	(copy_destination_begin),%edx
	movl	%edx,	(%esp)
	movl	(buffer_begin_address),%edx
	movl	%edx,	0x04(%esp)
	movl	(copy_size),%edx
	movl	%edx,	0x08(%esp)
	call	memcpy
	# continuation condition judgement
	movl	(end_disk_address),%eax
	movl	(last_disk_address),%edx
	cmpl	%edx,	%eax
	jae	11f
	# advance next disk range to read
	movl	$begin_disk_address,(%esp)
	movl	$end_disk_address,0x04(%esp)
	movl	$end_disk_address,%edx
	subl	$begin_disk_address,%edx
	movl	%edx,	0x08(%esp)
	call	memcpy
	jmp 5b
11:	# jump to kernel
	movl	$0x00300000,%ebp
	movl	$0x00300000,%esp
	ljmp	$0x0010,$0x00106e00

disk_address_2_sector_specifier:	# // void disk_address_2_sector_specifier(unsigned int disk_address, SectorSpecifier *sector_specifier);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%edi
1:
	movl	0x08(%ebp),%eax		# %eax = disk_address;
	movl	0x0c(%ebp),%edi		# %edi = sector_specifier;
	movl	$sector_size,%ecx	# %ecx = sector_size;
	xorl	%edx,	%edx		# %edx = 0;
	divl	%ecx			# %eax = disk_address / sector_size;
	movl	$track_size,%ecx	# %ecx = track_size;
	xorl	%edx,	%edx		# %edx = 0;
	divl	%ecx			# %eax = disk_address / sector_size / track_size;
					# %edx = disk_address / sector_size % track_size;
	incw	%dx			# %dx = disk_address / sector_size % track_size + 1;
	movw	%dx,	0x04(%edi)	# sector_specifier->sector = disk_address / sector_size % track_size + 1;
	movl	$heads,	%ecx		# %ecx = heads;
	xorl	%edx,	%edx		# %edx = 0;
	divl	%ecx			# %eax = disk_address / sector_size / track_size / heads;
					# %edx = disk_address / sector_size / track_size % heads;
	movw	%dx,	0x02(%edi)	# sector_specifier->head = disk_address / sector_size / track_size % heads;
	movw	%ax,	(%edi)		# sector_specifier->cylinder = disk_address / sector_size / track_size / heads;
2:
	popl	%edi
	leave
	ret

load_sectors_32:			# // void load_sectors_32(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushal
1:
	ljmp	$0x20,	$load_sectors_16
return_2_32:
0:
	movw	$0x08,	%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	popal
	leave
	ret

# void *memcpy
# (
#	void *destination,	// 0x08(%ebp)
#	void const *source,	// 0x0c(%ebp)
#	size_t size		// 0x10(%ebp)
# );
memcpy:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# Save the preserved registers.
	pushl	%ebx
	pushl	%edi
	pushl	%esi
2:	# Load the arguments.
	movl	0x08(%ebp),%edi		# EDI = destination;
	movl	0x0c(%ebp),%esi 	# ESI = source;
	movl	0x10(%ebp),%ecx 	# ECX = size;
3:	# Copy from source to destination.
	movl	%ecx,	%edx		# EDX = size;
	shrl	$0x02,	%ecx		# ECX = size / 4;
	andl	$0x00000003,%edx	# EDX = size % 4;
	rep	movsl			# while(ECX--)
					# {
					#	*(int *)EDI = *(int *)ESI;
					#	ESI += DF ? -4 : 4;
					#	EDI += DF ? -4 : 4;
					# }
	testl	%edx,	%edx		# if(!(size % 4))goto 5;
	jz	5f
	cmpl	$0x00000001,%edx	# if(size % 4 == 1)goto 4;
	je	4f
	movsw				# *(short *)EDI = *(short *)ESI;
					# ESI += DF ? -2 : 2;
					# EDI += DF ? -2 : 2;
	subl	$0x00000002,%edx	# EDX = size % 2;
	testl	%edx,	%edx		# if(!(size % 2))goto 5;
	jz	5f
4:	# Copy the last byte.
	movsb				# *(char *)EDI = *(char *)ESI;
					# ESI += DF ? -1 : 1;
					# EDI += DF ? -1 : 1;
5:	# Restore the preserved registers.
	popl	%esi
	popl	%edi
	popl	%ebx
6:	# End of the function.
	movl	0x08(%ebp),%eax		# return destination;
	leave
	ret

				# // print LF
new_line_serial:		# void new_line_serial(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp
	movl	$0x0000000a,(%esp)# print LF
	call	putchar_serial
	addl	$0x00000004,%esp
	leave
	ret

				# // print value as hexadecimal
print_byte_hex_serial:		# void print_byte_hex_serial(unsigned short value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000008,%esp
1:
	movl	$0x00000001,%ecx	# if %ecx == 1, then print the digit of 0x10s place, else print the digit of 0x01s place.
	movb	0x08(%ebp),%dl		# get the byte
	shrb	$0x04,	%dl
2:
	andb	$0x0f,%dl
	cmpb	$0x0a,	%dl
	jge	4f
3:					# the digit is less then 0x0a
	addb	$0x30,	%dl
	jmp	5f
4:					# the digit is greater than or equal to 0x0a
	subb	$0x0a,	%dl
	addb	$0x61,	%dl
5:					# print the digit
	movl	%ecx,	0x04(%esp)
	movl	%edx,	(%esp)
	call	putchar_serial
	movl	0x04(%esp),%ecx
	jcxz	6f
	movb	0x08(%ebp),%dl		# get the byte
	decl	%ecx
	jmp	2b
6:
	addl	$0x00000008,%esp
	leave
	ret

print_dword_hex_serial:			# void print_dword_hex_serial(unsigned int value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp
1:	# print higher word
	movl	0x08(%ebp),%edx		# %edx = value;
	shrl	$0x10,	%edx		# %edx = value >> 0x10;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
2:	# print lower word
	movl	0x08(%ebp),%edx		# %edx = value;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
3:
	addl	$0x00000004,%esp
	leave
	ret

print_word_hex_serial:			# void print_word_hex_serial(unsigned short value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp
1:	# print higher byte
	movw	0x08(%ebp),%dx		# %dx = value;
	shrw	$0x08,	%dx		# %dx = value >> 0x08;
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
2:	# print lower byte
	movw	0x08(%ebp),%dx		# %dx = value;
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
3:
	addl	$0x00000004,%esp
	leave
	ret

print_sector_specifier:		# void print_sector_specifier(SectorSpecifier *sector_specifier);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	subl	$0x00000004,%esp
	movl	0x08(%ebp),%esi
1:	# print cylinder number
	movl	$cylinder_message,(%esp)
	call	print_serial
	movw	(%esi),	%dx
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
2:	# print head number
	movl	$head_message,(%esp)
	call	print_serial
	movw	0x02(%esi),%dx
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
3:	# print sector number
	movl	$sector_message,(%esp)
	call	print_serial
	movw	0x04(%esi),%dx
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
4:
	addl	$0x00000004,%esp
	popl	%esi
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
	movzxb	(%esi),	%eax
	cmpb	$0x00,	%al
	je	2f		# finish putting all characters
	movl	%eax,	(%esp)
	call	putchar_serial
	incw	%si
	jmp	1b		# put next character
2:				# finish putting all characters
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
	movw	$com1,	%dx
	addw	$0x0005,%dx
	inb	%dx,	%al
	andb	$0x20,	%al
	jz	1b
2:				# send the character
	movb	0x08(%ebp),%al
	movw	$com1,	%dx
	outb	%al,	%dx
3:
	leave
	ret

sector_specifier_2_disk_address:	# unsigned int sector_specifier_2_disk_address(SectorSpecifier const *sector_specifier);
0:
	pushl	%ebp
	movl	%esp,	%ebp
1:
	movl	0x08(%ebp),%ecx 	# %ecx = sector_specifier;
	movzxw	(%ecx),	%eax		# %eax = sector_specifier->cylinder;
	movl	$heads,	%edx		# %edx = heads;
	mull	%edx			# %edx:%eax = sector_specifier->cylinder * heads;
	movzxw	0x02(%ecx),%edx		# %edx = sector_specifier->head;
	addl	%edx,	%eax		# %eax = sector_specifier->cylinder * heads + sector_specifier->head;
	movl	$track_size,%edx	# %edx = track_size;
	mull	%edx			# %edx:%eax = (sector_specifier->cylinder * heads + sector_specifier->head) * track_size;
	movzxw	0x04(%ecx),%edx		# %edx = sector_specifier->sector;
	decl	%edx			# %edx = sector_specifier->sector - 1;
	addl	%edx,	%eax		# %eax = (sector_specifier->cylinder * heads + sector_specifier->head) * track_size + sector_specifier->sector - 1;
	movl	$sector_size,%edx	# %edx = sector_size;
	mull	%edx			# %edx:%eax = ((sector_specifier->cylinder * heads + sector_specifier->head) * track_size + sector_specifier->sector - 1) * sector_size;
2:
	leave
	ret

validate_sector_specifier:		# void validate_sector_specifier(SectorSpecifier *sector_specifier);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%ebx
1:
	movl	0x08(%ebp),%ebx 	# %ebx = sector_specifier;
	movzxw	0x04(%ebx),%eax		# %ax = sector_specifier->sector;
	decw	%ax			# %ax = sector_specifier->sector - 1;
	movw	$track_size,%cx		# %cx = track_size;
	xorw	%dx,	%dx		# %dx = 0;
	divw	%cx			# %ax = (sector_specifier->sector - 1) / track_size;
					# %dx = (sector_specifier->sector - 1) % track_size;
	incw	%dx			# %dx = (sector_specifier->sector - 1) % track_size + 1;
	movw	%dx,	0x04(%ebx)	# sector_specifier->sector = (sectpr_specifier->sector - 1) % track_size + 1;
	addw	0x02(%ebx),%ax		# %ax += sector_specifier->head;
	movw	$heads,	%cx		# %cx = heads;
	xorw	%dx,	%dx		# %dx = 0;
	divw	%cx			# %ax = %ax / heads;
					# %dx = %ax % heads;
	movw	%dx,	0x02(%ebx)	# sector_specifier->head = %dx;
	addw	%ax,	(%ebx)		# sector_specifier->cylinder += %ax;
2:
	popl	%ebx
	leave
	ret

	.code16
load_sectors_16:	# 16bit protected mode
0:
	# set 16bit protected mode data segment
	movw	$0x0018,%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	# clear CR0 PE bit
	movl	%cr0,	%eax
	andl	$0x7ffffffe,%eax
	movl	%eax,	%cr0
	jmp	1f
1:	# 16bit real mode
	# set 16bit real mode data segment
	movw	$0x0000,%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	ljmp	$0x0000,$load_sectors
load_sectors:		# 16bit real mode
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	subw	$0x000c,%sp
	movw	%sp,	%bx
	# 0x00(%bx) = cylinder number
	# 0x02(%bx) = head
	# 0x04(%bx) = sector number
	# 0x06(%bx) = number of sectors to load
	# 0x08(%bx) = destination segment
	# 0x0a(%bx) = destination address
	# serial test
	movw	$real_mode_message,(%bx)
	call	print_serial_16
	call	new_line_serial_16
	call	new_line_serial_16
	# enable interrupt
	movb	$0xb8,%al
	outb	%al,	$0x0021
	movb	$0xbf,%al
	outb	%al,	$0x00a1
	sti
	# initialize load_sector arguments
	movw	(begin_cylinder),%dx
	movw	%dx,	(%bx)
	movw	(begin_head),%dx
	movw	%dx,	0x02(%bx)
	movw	(begin_sector),%dx
	movw	%dx,	0x04(%bx)
	movw	$0x0001,0x06(%bx)
	movw	(buffer_begin_segment),%dx
	movw	%dx,	0x08(%bx)
	movw	$0x0000,0x0a(%bx)
1:	# load loop
	# check load_sector arguments
	pushw	%bx
	call	print_sector_specifier_16
	pushw	$number_of_sectors_message
	call	print_serial_16
	pushw	0x06(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	pushw	$destination_segment_message
	call	print_serial_16
	pushw	0x08(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	pushw	$destination_address_message
	call	print_serial_16
	pushw	0x0a(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	movw	%bx,	%sp
	call	new_line_serial_16
	# load sector
	call	load_sector
	# increment sector
	movw	$sector_size,%dx
	shrw	$0x04,	%dx
	addw	%dx	,0x08(%bx)
	incw	0x04(%bx)
	pushw	%bx
	call	validate_sector_specifier_16
	movw	%bx,	%sp
	movw	(%bx),%dx
	cmpw	(end_cylinder),%dx
	jb 1b
	ja 2f
	movw	0x02(%bx),%dx
	cmpw	(end_head),%dx
	jb 1b
	ja 2f
	movw	0x04(%bx),%dx
	cmpw	(end_sector),%dx
	jb 1b
	ja 2f
2:
	# disable interrupt
	movb	$0xff,	%al
	outb	%al,	$0x0021
	outb	%al,	$0x00a1
	cli
	# closing
	addw	$0x000c,%sp
	popw	%bx
	leave
3:	# return to 16bit protected mode
	# set CR0 PE bit
	movl	%cr0,	%eax
	andl	$0x7fffffff,%eax
	orl	$0x00000001,%eax
	movl	%eax,	%cr0
	jmp	4f
4:
	# set 32bit protected mode data segment
	movw	$0x0008,%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	# return to 32 bit protected mode
	ljmp	$0x0010,$return_2_32

				# // load a sector from A drive
				# // cylinder_number: 0x0000~0x004f
				# // head: 0x0000, 0x0001
				# // sector_number: 0x0000~0x0012
				# // num_of_sectors: 0x0001~0x0012
				# // destination: [destination_segment:destination_address]
				# // return value 0 means success
				# // return value 1 means failure
load_sector:			# unsigned short load_sector(unsigned short cylinder_number, unsigned short head, unsigned short sector_number, unsigned short num_of_sectors, unsigned short destination_segment, unsigned short destination_address);
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
	movb	$0x02,	%ah	# load sectors
	movb	0x0a(%bp),%al	# number of loaded sectors
	movw	0x04(%bp),%cx	# cylinder_number
	rolw	$0x08,	%cx
	shlb	$0x06,	%cl
	addb	0x08(%bp),%cl	# sector_number
	xorb	%dl,	%dl	# load from A drive
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
	call	print_serial_16
	movw	0x02(%di),%ax
	movw	%ax,	(%di)
	call	print_byte_hex_serial_16
	call	new_line_serial_16
	movw	0x04(%di),%cx
	decw	%cx
	jnz	1b		# retry
	movw	$error_message,(%di)
	call	print_serial_16
	movw	$0x0001,%ax
4:
	addw	$0x0006,%sp
	popw	%es
	popw	%di
	popw	%bx
	leave
	ret

				# // print CRLF
new_line_serial_16:		# void new_line_serial_16(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000a,(%di)
	call	putchar_serial_16
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print value as hexadecimal
print_byte_hex_serial_16:	# void print_byte_hex_serial_16(unsigned char value);
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
	call	putchar_serial_16
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

print_dword_hex_serial_16:	# void print_dword_hex_serial_16(unsigned short low, unsigned short high);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x06(%bp),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial_16
	movw	0x04(%bp),%dx
	movw	%dx,	(%di)
	call	print_word_hex_serial_16
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print value as hexadecimal
print_word_hex_serial_16:	# void print_word_hex_serial_16(unsigned short value);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%dx
	movw	%dx,	0x02(%di)
	shr	$0x0008,%dx
	movw	%dx,	(%di)
	call	print_byte_hex_serial_16
	movw	0x02(%di),%dx
	andw	$0x00ff,%dx
	movw	%dx,	(%di)
	call	print_byte_hex_serial_16
	addw	$0x0004,%sp
	popw	%di
	leave
	ret

print_sector_specifier_16:		# void print_sector_specifier_16(SectorSpecifier *sector_specifier);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	pushw	%si
	subw	$0x0002,%sp
	movw	%sp,	%bx
	movw	0x04(%bp),%si
1:	# print cylinder number
	movw	$cylinder_message,(%bx)
	call	print_serial_16
	movw	(%si),	%dx
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
2:	# print head number
	movw	$head_message,(%bx)
	call	print_serial_16
	movw	0x02(%si),%dx
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
3:	# print sector number
	movw	$sector_message,(%bx)
	call	print_serial_16
	movw	0x04(%si),%dx
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
4:
	addw	$0x0002,%sp
	popw	%si
	popw	%bx
	leave
	ret

				# // print string to console
print_serial_16:		# void print_serial_16(char *string);
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
	call	putchar_serial_16
	incw	%si
	jmp	1b		# put next character
2:
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print a character to console
putchar_serial_16:		# void putchar_serial(char c);
0:
	pushw	%bp
	movw	%sp,	%bp
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
3:
	leave
	ret

validate_sector_specifier_16:		# void validate_sector_specifier_16(SectorSpecifier *sector_specifier);
0:
	pushw	%bp
	movw	%sp,	%bp
	push	%bx
1:
	movw	0x04(%bp),%bx 		# %bx = sector_specifier;
	movw	0x04(%bx),%ax		# %ax = sector_specifier->sector;
	decw	%ax			# %ax = sector_specifier->sector - 1;
	movw	$track_size,%cx		# %cx = track_size;
	xorw	%dx,	%dx		# %dx = 0;
	divw	%cx			# %ax = (sector_specifier->sector - 1) / track_size;
					# %dx = (sector_specifier->sector - 1) % track_size;
	incw	%dx			# %dx = (sector_specifier->sector - 1) % track_size + 1;
	movw	%dx,	0x04(%bx)	# sector_specifier->sector = (sectpr_specifier->sector - 1) % track_size + 1;
	addw	0x02(%bx),%ax		# %ax += sector_specifier->head;
	movw	$heads,	%cx		# %cx = heads;
	xorw	%dx,	%dx		# %dx = 0;
	divw	%cx			# %ax = %ax / heads;
					# %dx = %ax % heads;
	movw	%dx,	0x02(%bx)	# sector_specifier->head = %dx;
	addw	%ax,	(%bx)		# sector_specifier->cylinder += %ax;
2:
	popw	%bx
	leave
	ret

	.data
	.align	0x8
# GDT
gdt:
				# null segment
				# selector 0x0000 null segment descriptor
	.word	0x0000		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x00		#  access_right
	.byte	0x00		#  limit_high
	.byte	0x00		#  base_high

				# data segment for 32bit protected mode
				# selector 0x0008 whole memory is readable and writable
				# base	0x00000000
				# limit	0xffffffff
				# access_right 0x409a
	.word	0xffff		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x92		#  access_right
	.byte	0xcf		#  limit_high
	.byte	0x00		#  base_high

				# code segment for 32bit protected mode
				# selector 0x0010 whole memory is readable and executable
				# base	0x00000000
				# limit	0xffffffff
				# access_right 0x4092
	.word	0xffff		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x9a		#  access_right
	.byte	0xcf		#  limit_high
	.byte	0x00		#  base_high

				# data segment for 16bit protected mode
				# selector 0x0018 whole memory is readable and writable
				# base	0x00000000
				# limit	0xffffffff
				# access_right 0x409a
	.word	0xffff		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x92		#  access_right
	.byte	0x0f		#  limit_high
	.byte	0x00		#  base_high

				# code segment for 16bit protected mode
				# selector 0x0020 whole memory is readable and executable
				# base	0x00000000
				# limit	0xffffffff
				# access_right 0x4092
	.word	0xffff		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x9a		#  access_right
	.byte	0x0f		#  limit_high
	.byte	0x00		#  base_high

gdtr:
	.word	(gdtr) - (gdt) - 1	# limit of GDT
	.long	gdt
# The floppy disk limit
last_disk_address:
	.long	sectors * sector_size
last_cylinder:
	.word	sectors / heads / track_size - 1
last_head:
	.word	heads - 1
last_sector:
	.word	track_size
# Buffer range
buffer_begin_segment:
	.word	0x1000
buffer_end_segment:
	.word	0x0000
buffer_size_segment:
	.word	0x0000
buffer_begin_address:
	.long	0x00000000
buffer_end_address:
	.long	0x00000000
buffer_size_address:
	.long	0x00000000
# Destination
destination:
	.long	0x00100000
# Next disk range to read
begin_disk_address:
	.long	0x00000000
begin_cylinder:
	.word	0x00
begin_head:
	.word	0x00
begin_sector:
	.word	0x00
end_disk_address:
	.long	0x00000000
end_cylinder:
	.word	0x00
end_head:
	.word	0x00
end_sector:
	.word	0x00
# Next Copy target
copy_destination_begin:
	.long	0x00000000
copy_destination_end:
	.long	0x00000000
copy_size:
	.long	0x00000000
# Serial messages
begin_disk_address_message:
	.string "begin_disk_address = 0x"
begin_sector_message:
	.string "begin_sector"
buffer_begin_address_message:
	.string "buffer_begin_address = 0x"
buffer_begin_segment_message:
	.string "buffer_begin_segment = 0x"
buffer_end_address_message:
	.string "buffer_end_address = 0x"
buffer_end_segment_message:
	.string "buffer_end_segment = 0x"
buffer_size_address_message:
	.string "buffer_size_address = 0x"
buffer_size_segment_message:
	.string "buffer_size_segment = 0x"
copy_destination_begin_message:
	.string "copy_destination_begin = 0x"
copy_destination_end_message:
	.string "copy_destination_end = 0x"
copy_size_message:
	.string "copy_size = 0x"
cylinder_message:
	.string "cylinder = 0x"
destination_address_message:
	.string "destination address = 0x"
destination_segment_message:
	.string "destination segment = 0x"
end_disk_address_message:
	.string "end_disk_address = 0x"
end_sector_message:
	.string "end_sector"
error_message:
	.string "ERROR!\n"
head_message:
	.string "head = 0x"
hello_message:
	.string "Hello, lddskxtr.bin!\n\n"
int13_error_message:
	.string "INT 0x13 ERROR AH = 0x"
last_disk_address_message:
	.string "last_disk_address = 0x"
last_sector_message:
	.string "last_cylinder"
number_of_sectors_message:
	.string "number_of_sectors = 0x"
protected_mode_message:
	.string "PROTECTED MODE NOW!"
real_mode_message:
	.string "REAL MODE NOW!"
sector_message:
	.string "sector = 0x"
	.align	0x0200
kernel:

