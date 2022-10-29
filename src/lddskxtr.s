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
# 	unsigned char cylinder;
# 	unsigned char head;
# 	unsigned char sector;
# } SectorSpecifier;

	.include	"global.s"

	.globl	main
	.globl	disk_address_2_sector_specifier
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

	.type	main,				@function
	.type	disk_address_2_sector_specifier,@function
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

	.code32
	.text
main:
0:
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
	movl	$last_disk_address,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
3:	# print buffer range
	# print buffer_begin
	movl	$buffer_begin_message,(%esp)
	call	print_serial
	movl	$buffer_begin,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print buffer_end
	movl	$buffer_end_message,(%esp)
	call	print_serial
	movl	$buffer_end,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print buffer_size
	movl	$buffer_size_message,(%esp)
	call	print_serial
	movl	$buffer_size,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	call	new_line_serial
4:	# calculate the first disk range to read
	# get last loaded cylinder
	movl	$last_loaded_cylinder,%esi
	movl	$begin_cylinder,%edi
	movb	(%esi),	%dl
	movb	%dl,	(%edi)
	# get last loaded head
	movl	$last_loaded_head,%esi
	movl	$begin_head,%edi
	movb	(%esi),	%dl
	movb	%dl,	(%edi)
	# get last loaded sector
	movl	$last_loaded_sector,%esi
	movl	$begin_sector,%edi
	movb	(%esi),	%dl
	incb	%dl		# calculate next sector
	movb	%dl,	(%edi)
	# standardize the begin sector specifier
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
	movl	$begin_disk_address,%edi
	movl	%eax,	(%edi)
	# check begin_disk_address
	movl	$begin_disk_address_message,(%esp)
	call	print_serial
	movl	$begin_disk_address,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
7:	# calculate end_disk_address
	movl	$begin_disk_address,%esi
	movl	(%esi),	%eax
	movl	$buffer_size,%esi
	movl	(%esi),	%edx
	addl	%edx,	%eax
	movl	$last_disk_address,%esi
	movl	(%esi),	%edx
	cmpl	%edx,	%eax
	jb	8f
	movl	%edx,	%eax
8:
	movl	$end_disk_address,%edi
	movl	%eax,	(%edi)
	movl	$end_cylinder,0x04(%esp)
	movl	%eax,	(%esp)
	call	disk_address_2_sector_specifier
	# print end_disk_address
	movl	$end_sector_message,(%esp)
	call	print_serial
	call	new_line_serial
	movl	$end_cylinder,(%esp)
	call	print_sector_specifier
	movl	$end_disk_address_message,(%esp)
	call	print_serial
	movl	$end_disk_address,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
9:	# calculate copy destination
	movl	$destination,%esi
	movl	(%esi),	%eax
	movl	$begin_disk_address,%esi
	movl	(%esi),	%ebx
	movl	$end_disk_address,%esi
	movl	(%esi),	%ecx
	movl	%eax,	%edx
	addl	%ecx,	%edx
	addl	%ebx,	%eax
	subl	%ebx,	%ecx
	movl	$copy_destination_begin,%edi
	movl	%eax,	(%edi)
	movl	$copy_destination_end,%edi
	movl	%edx,	(%edi)
	movl	$copy_size,%edi
	movl	%ecx,	(%edi)
	# print copy destination
	movl	$copy_destination_begin_message,(%esp)
	call	print_serial
	movl	$copy_destination_begin,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	movl	$copy_destination_end_message,(%esp)
	call	print_serial
	movl	$copy_destination_end,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	movl	$copy_size_message,(%esp)
	call	print_serial
	movl	$copy_size,%esi
	movl	(%esi),	%edx
	movl	%edx,	(%esp)
	call	print_dword_hex_serial
	call	new_line_serial
	call	new_line_serial
10:	# read sectors and copy to destination
	# copy to destination
	movl	$copy_destination_begin,(%esp)
	movl	$buffer_begin,0x04(%esp)
	movl	$copy_size,0x08(%esp)
	call	memcpy
	# continuation condition judgement
	movl	$end_disk_address,(%esi)
	movl	(%esi),	%eax
	movl	$last_disk_address,(%esi)
	movl	(%esi),	%edx
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
11:
	addl	$0x0000000c,%esp
	hlt
	jmp	2b
11:	# jump to kernel
	movl	$0x00300000,%ebp
	movl	$0x00300000,%esp
	jmp	kernel

disk_address_2_sector_specifier:	# // void (unsigned int disk_address, SectorSpecifier *sector_specifier);
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
	incb	%dl			# %dl = disk_address / sector_size % track_size + 1;
	movb	%dl,	0x02(%edi)	# sector_specifier->sector = disk_address / sector_size % track_size + 1;
	movl	$heads,	%ecx		# %ecx = heads;
	xorl	%edx,	%edx		# %edx = 0;
	divl	%ecx			# %eax = disk_address / sector_size / track_size / heads;
					# %edx = disk_address / sector_size / track_size % heads;
	movb	%dl,	0x01(%edi)	# sector_specifier->head = disk_address / sector_size / track_size % heads;
	movb	%al,	(%edi)		# sector_specifier->cylinder = disk_address / sector_size / track_size / heads;
2:
	popl	%edi
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
	subl	$0x00000004,%esp
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
	movl	%ecx,	%ebx
	movl	%edx,	(%esp)
	call	putchar_serial
	movl	%ebx,	%ecx
	jcxz	6f
	movb	0x08(%ebp),%dl		# get the byte
	decl	%ecx
	jmp	2b
6:
	addl	$0x00000004,%esp
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
	movb	(%esi),	%dl
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
2:	# print head number
	movl	$head_message,(%esp)
	call	print_serial
	movb	0x01(%esi),%dl
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
3:	# print sector number
	movl	$sector_message,(%esp)
	call	print_serial
	movb	0x02(%esi),%dl
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
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
	movzxb	(%ecx),	%eax		# %eax = sector_specifier->cylinder;
	movl	$heads,	%edx		# %edx = heads;
	mull	%edx			# %edx:%eax = sector_specifier->cylinder * heads;
	movzxb	0x01(%ecx),%edx		# %edx = sector_specifier->head;
	addl	%edx,	%eax		# %eax = sector_specifier->cylinder * heads + sector_specifier->head;
	movl	$track_size,%edx	# %edx = track_size;
	mull	%edx			# %edx:%eax = (sector_specifier->cylinder * heads + sector_specifier->head) * track_size;
	movzxb	0x02(%ecx),%edx		# %edx = sector_specifier->sector;
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
1:
	movl	0x08(%ebp),%ecx 	# %ecx = sector_specifier;
	movzxb	0x02(%ecx),%eax		# %al = sector_specifier->sector;
	decb	%al			# %al = sector_specifier->sector - 1;
	movb	$track_size,%dl		# %dl = track_size;
	divb	%dl			# %al = (sector_specifier->sector - 1) / track_size;
					# %ah = (sector_specifier->sector - 1) % track_size;
	incb	%ah			# %ah = (sector_specifier->sector - 1) % track_size + 1;
	movb	%ah,	0x02(%ecx)	# sector_specifier->sector = (sectpr_specifier->sector - 1) % track_size + 1;
	addb	0x01(%ecx),%al		# %al += sector_specifier->head;
	xorb	%ah,	%ah		# %ah = 0;
	movb	$heads,	%dl		# %dl = heads;
	divb	%dl			# %al = %al / heads;
					# %ah = %al % heads;
	movb	%ah,	0x01(%ecx)	# sector_specifier->head = %ah;
	addb	%al,	(%ecx)		# sector_specifier->cylinder += %al;
2:
	leave
	ret

	.data
	.align	0x8
# GDT for 32bit protected mode
gdt32:
				# selector 0x0000 null segment descriptor
	.word	0x0000		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x00		#  access_right
	.byte	0x00		#  limit_high
	.byte	0x00		#  base_high

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
gdtr32:
	.word	0x0017		# 3 segment descriptors * 8 bytes per segment descriptor - 1
	.long	gdt32
# The floppy disk limit
last_disk_address:
	.long	sectors * sector_size
last_cylinder:
	.byte	sectors / heads / track_size - 1
last_head:
	.byte	heads - 1
last_sector:
	.byte	track_size
# Buffer range
buffer_begin:
	.word	load_dest
buffer_end:
	.word	main
buffer_size:
	.word	main - load_dest
# Destination
destination:
	.long	0x00100000
# Next disk range to read
begin_disk_address:
	.long	0x00000000
begin_cylinder:
	.byte	0x00
begin_head:
	.byte	0x00
begin_sector:
	.byte	0x00
end_disk_address:
	.long	0x00000000
end_cylinder:
	.byte	0x00
end_head:
	.byte	0x00
end_sector:
	.byte	0x00
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
buffer_begin_message:
	.string "buffer_begin = 0x"
buffer_end_message:
	.string "buffer_end = 0x"
buffer_size_message:
	.string "buffer_size = 0x"
copy_destination_begin_message:
	.string "copy_destination_begin = 0x"
copy_destination_end_message:
	.string "copy_destination_end = 0x"
copy_size_message:
	.string "copy_size = 0x"
cylinder_message:
	.string "cylinder = 0x"
end_disk_address_message:
	.string "end_disk_address = 0x"
end_sector_message:
	.string "end_sector"
head_message:
	.string "head = 0x"
hello_message:
	.string "Hello, lddskxtr.bin!\n\n"
last_disk_address_message:
	.string "last_disk_address = 0x"
last_sector_message:
	.string "last_cylinder"
sector_message:
	.string "sector = 0x"
	.align	0x0200
kernel:

