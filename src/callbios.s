# This program is called from kernel.bin to call bios functions.
# This program switch from protected mode to real mode, call bios function, and return to protected mode.
#
# calling convention = System V i386
# return value: eax, edx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.include	"global.s"

	# 32bit mode functions
	.globl	call_bios
	.globl	return_2_32
	.globl	new_line_serial
	.globl	print_byte_hex_serial
	.globl	print_dword_hex_serial
	.globl	print_word_hex_serial
	.globl	print_serial
	.globl	putchar_serial
	# 16bit mode functions
	.globl	call_bios_16

	# 32bit mode functions
	.type	call_bios,		@function
	.type	return_2_32,		@function
	.type	new_line_serial,	@function
	.type	print_byte_hex_serial,	@function
	.type	print_dword_hex_serial,	@function
	.type	print_word_hex_serial,	@function
	.type	print_serial,		@function
	.type	putchar_serial,		@function
	# 16bit mode functions
	.type	call_bios_16,		@function

	.text
stack_floor:
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
	# deploy arguments
	movb	0x08(%ebp),%dl
	movb	%dl,	(interrupt_number)
	movl	0x0c(%ebp),%ebx	# ebx = arguments;
	movw	(%ebx),	%dx
	movw	%dx,	(argument_ax)
	movw	0x02(%ebx),%dx
	movw	%dx,	(argument_cx)
	movw	0x04(%ebx),%dx
	movw	%dx,	(argument_bx)
	movw	0x06(%ebx),%dx
	movw	%dx,	(argument_dx)
	movw	0x08(%ebx),%dx
	movw	%dx,	(argument_si)
	movw	0x0a(%ebx),%dx
	movw	%dx,	(argument_di)
	movw	0x0c(%ebx),%dx
	movw	%dx,	(argument_es)
	movw	0x0e(%ebx),%dx
	movw	%dx,	(argument_fs)
	movw	0x10(%ebx),%dx
	movw	%dx,	(argument_gs)
2:
	# print interrupt_number
	movl	$interrupt_number_message,(%esp)
	call	print_serial
	movb	(interrupt_number),%dl	# dl = interrupt_number;
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
	# print arguments->ax
	movl	$arguments_ax_message,(%esp)
	call	print_serial
	movw	(argument_ax),%dx	# dx = arguments->ax;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->cx
	movl	$arguments_cx_message,(%esp)
	call	print_serial
	movw	(argument_cx),%dx	# dx = arguments->cx;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->bx
	movl	$arguments_bx_message,(%esp)
	call	print_serial
	movw	(argument_bx),%dx	# dx = arguments->bx;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->dx
	movl	$arguments_dx_message,(%esp)
	call	print_serial
	movw	(argument_dx),%dx	# dx = arguments->dx;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->si
	movl	$arguments_si_message,(%esp)
	call	print_serial
	movw	(argument_si),%dx	# dx = arguments->si;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->di
	movl	$arguments_di_message,(%esp)
	call	print_serial
	movw	(argument_di),%dx	# dx = arguments->di;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->es
	movl	$arguments_es_message,(%esp)
	call	print_serial
	movw	(argument_es),%dx	# dx = arguments->es;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->fs
	movl	$arguments_fs_message,(%esp)
	call	print_serial
	movw	(argument_fs),%dx	# dx = arguments->fs;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print arguments->gs
	movl	$arguments_gs_message,(%esp)
	call	print_serial
	movw	(argument_gs),%dx	# dx = arguments->gs;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
3:
	pushal				# save registers
	movl	%esp,	(esp_32)	# save esp
	lgdt	(gdtr_16)		# switch GDT
	jmp	$0x20,	$call_bios_16
return_2_32:
0:
	movw	$0x08,	%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	lgdt	(gdtr_32)		# restore GDT
	movl	(esp_32),%esp		# restore esp
	popal				# restore registers
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

	.code16

call_bios_16:
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
2:	# return to 16bit protected mode
	# set CR0 PE bit
	movl	%cr0,	%eax
	andl	$0x7fffffff,%eax
	orl	$0x00000001,%eax
	movl	%eax,	%cr0
	jmp	3f
3:
	# set 32bit protected mode data segment
	movw	$0x0008,%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	# return to 32 bit protected mode
	jmp	$0x0010,$return_2_32

	.data
	.align	0x8
# GDT
gdt_16:
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
				# selector 0x0008 whole memory is readable and writable
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
				# selector 0x0010 whole memory is readable and executable
				# base	0x00000000
				# limit	0xffffffff
				# access_right 0x4092
	.word	0xffff		#  limit_low
	.word	0x0000		#  base_low
	.byte	0x00		#  base_mid
	.byte	0x9a		#  access_right
	.byte	0x0f		#  limit_high
	.byte	0x00		#  base_high

gdtr_16:
	.word	(gdtr_16) - (gdt_16) - 1	# limit of GDT
	.long	gdt_16

gdtr_32:
	.word	0xffff
	.long	0x00270000

esp_32:
	.long	0x00000000

interrupt_number:
	.byte	0x00
argument_ax:
	.word	0x0000
argument_cx:
	.word	0x0000
argument_bx:
	.word	0x0000
argument_dx:
	.word	0x0000
argument_si:
	.word	0x0000
argument_di:
	.word	0x0000
argument_es:
	.word	0x0000
argument_fs:
	.word	0x0000
argument_gs:
	.word	0x0000
interrupt_number_message:
	.string "interrupt_num = 0x"
arguments_ax_message:
	.string "arguments->ax = 0x"
arguments_cx_message:
	.string "arguments->cx = 0x"
arguments_bx_message:
	.string "arguments->bx = 0x"
arguments_dx_message:
	.string "arguments->dx = 0x"
arguments_si_message:
	.string "arguments->si = 0x"
arguments_di_message:
	.string "arguments->di = 0x"
arguments_es_message:
	.string "arguments->es = 0x"
arguments_fs_message:
	.string "arguments->fs = 0x"
arguments_gs_message:
	.string "arguments->gs = 0x"
