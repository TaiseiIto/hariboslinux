# This program is called from dplydisk.s to load the entire floppy disk switching back and forth between 16bit mode and 32bit mode.
#
# Source disk address        0x00000000~0x00167fff
# Destination memory address 0x00100000~0x00267fff
# Load buffer                0x00007c00~0x0000dc00
#
# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.globl	main
	.globl	new_line_serial
	.globl	print_byte_hex_serial
	.globl	print_serial
	.globl	putchar_serial
	.globl	validate_sector_specifier

	.type	main,				@function
	.type	new_line_serial,		@function
	.type	print_byte_hex_serial,		@function
	.type	print_serial,			@function
	.type	putchar_serial,			@function
	.type	validate_sector_specifier,	@function

	.code32
	.text
main:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000004,%esp
1:				# print test message
	call	new_line_serial
	movl	$hello_message,(%esp)
	call	print_serial
2:				# calculate the first disk range to read
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
3:
	# check begin cylinder
	movl	$begin_cylinder_message,(%esp)
	call	print_serial
	movl	$begin_cylinder,%esi
	movb	(%esi),	%dl
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
	# check begin head
	movl	$begin_head_message,(%esp)
	call	print_serial
	movl	$begin_head,%esi
	movb	(%esi),	%dl
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
	# check begin sector
	movl	$begin_sector_message,(%esp)
	call	print_serial
	movl	$begin_sector,%esi
	movb	(%esi),	%dl
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
3:
	addl	$0x00000004,%esp
	hlt
	jmp	2b
4:				# jump to kernel
	movl	$0x00300000,%ebp
	movl	$0x00300000,%esp
	jmp	kernel

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

				# // print string to serial port COM1
print_serial:			# void print_serial(char *string);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	subl	$0x00000004,%esp
	movl	0x08(%ebp),%esi
1:				# put loop
	xorl	%eax,	%eax
	movb	(%esi),	%al
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

				# typedef struct
				# {
				# 	unsigned char cylinder;
				# 	unsigned char head;
				# 	unsigned char sector;
				# } SectorSpecifier;
validate_sector_specifier:	# void validate_sector_specifier(SectorSpecifier *sector_specifier);
0:
	pushl	%ebp
	movl	%esp,	%ebp
1:
	movl	0x08(%ebp),%esi # %esi = sector_specifier;
	movl	%esi,	%edi	# %edi = sector_specifier;
	movb	0x02(%esi),%al	# %al = sector;
	decb	%al		# %al = sector - 1;
	xorb	%ah,	%ah	# %ah = 0;
	movb	$track_size,%dl	# %dl = track_size;
	divb	%dl		# %al = (sector - 1) / track_size;
				# %ah = (sector - 1) % track_size;
	incb	%ah		# %ah = (sector - 1) % track_size + 1;
	movb	%ah,	0x02(%edi) # sector = (sector - 1) % track_size + 1;
	addb	0x01(%esi),%al	# %al += head;
	xorb	%ah,	%ah	# %ah = 0;
	movb	$heads,	%dl	# %dl = heads;
	divb	%dl		# %al = %al / heads;
				# %ah = %al % heads;
	movb	%ah,	0x01(%edi) # head = %ah;
	addb	%al,	(%esi)	# cylinder += %al;
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
copy_destination:
	.long	0x00000000
copy_size:
	.long	0x00000000
# Serial messages
begin_cylinder_message:
	.string "begin_cylinder = 0x"
begin_head_message:
	.string "begin_head = 0x"
begin_sector_message:
	.string "begin_sector = 0x"
hello_message:
	.string "Hello, lddskxtr.bin!\n\n"
	.align	0x0200
kernel:

