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
	.globl	print_serial
	.globl	putchar_serial

	.type	main,		@function
	.type	new_line_serial,@function
	.type	print_serial,	@function
	.type	putchar_serial,	@function

	.code32
	.text
main:
0:
	pushl	%ebp
	movl	%esp,	%ebp
1:
	call	new_line_serial
	movl	$hello_message,(%esp)
	call	print_serial
2:
	hlt
	jmp	2b

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
begin_cylinder:
	.byte	0x00
begin_head:
	.byte	0x00
begin_sector:
	.byte	0x00
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
# Test message
hello_message:
	.string "Hello, lddskxtr.bin!\n\n"
	.align	0x0200
kernel:

