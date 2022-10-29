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
hello_message:
	.string "Hello, lddskxtr.bin!\n\n"
	.align	0x0200
kernel:

