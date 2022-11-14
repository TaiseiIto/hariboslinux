# This program is called from kernel.bin to call bios functions.
# This program switch from protected mode to real mode, call bios function, and return to protected mode.
#
# calling convention = System V i386
# return value: eax, edx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.include	"global.s"

	.globl	call_bios
	.globl	new_line_serial
	.globl	print_byte_hex_serial
	.globl	print_dword_hex_serial
	.globl	print_word_hex_serial
	.globl	print_serial
	.globl	putchar_serial

	.type	call_bios,		@function
	.type	new_line_serial,	@function
	.type	print_byte_hex_serial,	@function
	.type	print_dword_hex_serial,	@function
	.type	print_word_hex_serial,	@function
	.type	print_serial,		@function
	.type	putchar_serial,		@function

	.text
	.code32

# typedef struct _BIOSInterface
# {
# 	unsigned short ax;
# 	unsigned short cx;
# 	unsigned short bx;
# 	unsigned short dx;
# 	unsigned short si;
# 	unsigned short di;
# 	unsigned short es;
# 	unsigned short fs;
# 	unsigned short gs;
# } BIOSInterface;

call_bios:			# BIOSInterface *call_bios(unsigned char interrupt_number, BIOSInterface *arguments);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%ebx
	subl	$0x00000004,%esp
1:
	# print interrupt_number
	movl	$interrupt_number_message,(%esp)
	call	print_serial
	movb	0x08(%ebp),%dl	# dl = interrupt_number;
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
	# print arguments
	movl	0x0c(%ebp),%ebx	# ebx = arguments;
	movl	$arguments_ax_message,(%esp)
	call	print_serial
	movw	(%ebx),	%dx	# dx = arguments->ax;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
2:
	addl	$0x00000004,%esp
	popl	%ebx
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

	.data
interrupt_number_message:
	.string "interrupt_num = 0x"
arguments_ax_message:
	.string "arguments->ax = 0x"

