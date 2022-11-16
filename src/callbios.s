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
	.globl	jmp_2_origin_cs
	.globl	origin_cs
	.globl	new_line_serial
	.globl	print_byte_hex_serial
	.globl	print_dword_hex_serial
	.globl	print_word_hex_serial
	.globl	print_serial
	.globl	putchar_serial
	# 16bit mode functions
	.globl	call_bios_16
	.globl	call_bios_16_real
	.globl	call_int
	.globl	new_line_serial_16
	.globl	print_byte_hex_serial_16
	.globl	print_dword_hex_serial_16
	.globl	print_word_hex_serial_16
	.globl	print_serial_16
	.globl	putchar_serial_16

	# 32bit mode functions
	.type	call_bios,			@function
	.type	return_2_32,			@function
	.type	jmp_2_origin_cs,		@function
	.type	origin_cs,			@function
	.type	new_line_serial,		@function
	.type	print_byte_hex_serial,		@function
	.type	print_dword_hex_serial,		@function
	.type	print_word_hex_serial,		@function
	.type	print_serial,			@function
	.type	putchar_serial,			@function
	# 16bit mode functions
	.type	call_bios_16,			@function
	.type	call_bios_16_real,		@function
	.type	call_int,			@function
	.type	new_line_serial_16,		@function
	.type	print_byte_hex_serial_16,	@function
	.type	print_dword_hex_serial_16,	@function
	.type	print_word_hex_serial_16,	@function
	.type	print_serial_16,		@function
	.type	putchar_serial_16,		@function

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

call_bios:			# BIOSInterface *call_bios(unsigned char interrupt_number, BIOSInterface *input, unsigned short task_status_segment_selector);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%ebx
	subl	$0x00000004,%esp
1:
	# deploy input
	movb	0x08(%ebp),%dl
	movb	%dl,	(interrupt_number)
	movl	0x0c(%ebp),%ebx	# ebx = input;
	movw	(%ebx),	%dx
	movw	%dx,	(input_ax)
	movw	0x02(%ebx),%dx
	movw	%dx,	(input_cx)
	movw	0x04(%ebx),%dx
	movw	%dx,	(input_bx)
	movw	0x06(%ebx),%dx
	movw	%dx,	(input_dx)
	movw	0x08(%ebx),%dx
	movw	%dx,	(input_si)
	movw	0x0a(%ebx),%dx
	movw	%dx,	(input_di)
	movw	0x0c(%ebx),%dx
	movw	%dx,	(input_bp)
	movw	0x0e(%ebx),%dx
	movw	%dx,	(input_es)
2:
	# print interrupt_number
	movl	$interrupt_number_message,(%esp)
	call	print_serial
	movb	(interrupt_number),%dl	# dl = interrupt_number;
	movb	%dl,	(%esp)
	call	print_byte_hex_serial
	call	new_line_serial
	# print input->ax
	movl	$input_ax_message,(%esp)
	call	print_serial
	movw	(input_ax),%dx	# dx = input->ax;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->cx
	movl	$input_cx_message,(%esp)
	call	print_serial
	movw	(input_cx),%dx	# dx = input->cx;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->bx
	movl	$input_bx_message,(%esp)
	call	print_serial
	movw	(input_bx),%dx	# dx = input->bx;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->dx
	movl	$input_dx_message,(%esp)
	call	print_serial
	movw	(input_dx),%dx	# dx = input->dx;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->si
	movl	$input_si_message,(%esp)
	call	print_serial
	movw	(input_si),%dx	# dx = input->si;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->di
	movl	$input_di_message,(%esp)
	call	print_serial
	movw	(input_di),%dx	# dx = input->di;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->bp
	movl	$input_bp_message,(%esp)
	call	print_serial
	movw	(input_bp),%dx	# dx = input->bp;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
	# print input->es
	movl	$input_es_message,(%esp)
	call	print_serial
	movw	(input_es),%dx	# dx = input->es;
	movw	%dx,	(%esp)
	call	print_word_hex_serial
	call	new_line_serial
3:
	pushal				# save registers
	movl	%cr0,	%eax
	pushl	%eax			# save cr0
	pushfl				# save eflags
	cli				# prohibit interruption
	movl	%esp,	(esp_32)	# save esp
	movw	%cs,	(jmp_2_origin_cs + 0x05)# save cs
	movw	%ss,	(ss_32)		# save ss
	movw	%ds,	(ds_32)		# save ds
	movw	%es,	(es_32)		# save es
	movw	%fs,	(fs_32)		# save fs
	movw	%gs,	(gs_32)		# save gs
	sgdt	(gdtr_32)		# save GDT
	lgdt	(gdtr_16)		# switch GDT
	sidt	(idtr_32)		# save IDT
	lidt	(idtr_16)		# switch IDT
	ljmp	$0x20,	$call_bios_16
return_2_32:
0:
	lidt	(idtr_32)		# restore IDT
	lgdt	(gdtr_32)		# restore GDT
jmp_2_origin_cs:
0:
	ljmp	$0xffff,$origin_cs
origin_cs:
0:
	movw	(ss_32),%ss		# restore ss
	movw	(ds_32),%ds		# restore ds
	movw	(es_32),%es		# restore es
	movw	(fs_32),%fs		# restore fs
	movw	(gs_32),%gs		# restore gs
	movl	(esp_32),%esp		# restore esp
	popfl				# restore eflags
	popl	%eax
	movl	%eax,	%cr0		# restore cr0
	popal				# restore registers
1:
	movl	$protected_mode_message,(%esp)
	call	print_serial
	call	new_line_serial
	movl	0x0c(%ebp),%eax		# eax = input;
	movw	(output_ax),%dx
	movw	%dx,	(%eax)		# input->ax = *output_ax;
	movw	(output_cx),%dx
	movw	%dx,	0x02(%eax)	# input->cx = *output_cx;
	movw	(output_bx),%dx
	movw	%dx,	0x04(%eax)	# input->bx = *output_bx;
	movw	(output_dx),%dx
	movw	%dx,	0x06(%eax)	# input->dx = *output_dx;
	movw	(output_cx),%dx
	movw	%dx,	0x02(%eax)	# input->cx = *output_cx;
	movw	(output_flags),%dx
	movw	%dx,	0x10(%eax)	# input->flags = *output_flags;
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
	ljmp	$0x0000,$call_bios_16_real
call_bios_16_real:	# set real mode stack
0:
	movw	$call_bios - 0x200,%bp
	movw	%bp,	%sp
1:	# make new stack frame
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	subw	$0x0002,%sp
	movw	%sp,	%bx
2:	# check input
	movw	$real_mode_message,(%bx)
	call	print_serial_16
	call	new_line_serial_16
	# print interrupt_number
	movl	$interrupt_number_message,(%bx)
	call	print_serial_16
	movb	(interrupt_number),%dl	# dl = interrupt_number;
	movb	%dl,	(%bx)
	call	print_byte_hex_serial_16
	call	new_line_serial_16
	# print input->ax
	movl	$input_ax_message,(%bx)
	call	print_serial_16
	movw	(input_ax),%dx	# dx = input->ax;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->cx
	movl	$input_cx_message,(%bx)
	call	print_serial_16
	movw	(input_cx),%dx	# dx = input->cx;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->bx
	movl	$input_bx_message,(%bx)
	call	print_serial_16
	movw	(input_bx),%dx	# dx = input->bx;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->dx
	movl	$input_dx_message,(%bx)
	call	print_serial_16
	movw	(input_dx),%dx	# dx = input->dx;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->si
	movl	$input_si_message,(%bx)
	call	print_serial_16
	movw	(input_si),%dx	# dx = input->si;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->di
	movl	$input_di_message,(%bx)
	call	print_serial_16
	movw	(input_di),%dx	# dx = input->di;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->bp
	movl	$input_bp_message,(%bx)
	call	print_serial_16
	movw	(input_bp),%dx	# dx = input->bp;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
	# print input->es
	movl	$input_es_message,(%bx)
	call	print_serial_16
	movw	(input_es),%dx	# dx = input->es;
	movw	%dx,	(%bx)
	call	print_word_hex_serial_16
	call	new_line_serial_16
3:	# save registers
	pushaw
6:	# PIC setting
	movb	$0x11,	%al
	outb	%al,	$0x0020
	movb	$0x08,	%al
	outb	%al,	$0x0021
	movb	$0x04,	%al
	outb	%al,	$0x0021
	movb	$0x01,	%al
	outb	%al,	$0x0021
	movb	$0x11,	%al
	outb	%al,	$0x00a0
	movb	$0x10,	%al
	outb	%al,	$0x00a1
	movb	$0x02,	%al
	outb	%al,	$0x00a1
	movb	$0x01,	%al
	outb	%al,	$0x00a1
	movb	$0xb8,	%al
	outb	%al,	$0x0021
	movb	$0xbf,	%al
	outb	%al,	$0x00a1
	sti
5:	# call bios
	movb	(interrupt_number),%dl
	movb	%dl,	(call_int + 1)
	movw	(input_ax),%ax
	movw	(input_cx),%cx
	movw	(input_bx),%bx
	movw	(input_dx),%dx
	movw	(input_si),%si
	movw	(input_di),%di
	movw	(input_bp),%bp
	movw	(input_es),%es
call_int:
0:
	int	$0xff
2:	# save output
	movw	%ax,	(output_ax)
	movw	%cx,	(output_cx)
	movw	%bx,	(output_bx)
	movw	%dx,	(output_dx)
	pushfw
	popw	%ax
	movw	%ax,	(output_flags)
1:	# PIC setting
	cli
	movb	$0x11,	%al
	outb	%al,	$0x0020
	movb	$0x20,	%al
	outb	%al,	$0x0021
	movb	$0x04,	%al
	outb	%al,	$0x0021
	movb	$0x01,	%al
	outb	%al,	$0x0021
	movb	$0x11,	%al
	outb	%al,	$0x00a0
	movb	$0x28,	%al
	outb	%al,	$0x00a1
	movb	$0x02,	%al
	outb	%al,	$0x00a1
	movb	$0x01,	%al
	outb	%al,	$0x00a1
	movb	$0xe8,	%al
	outb	%al,	$0x0021
	movb	$0xee,	%al
	outb	%al,	$0x00a1
2:	# restore registers
	popaw
3:	# clean stack frame
	addw	$0x0002,%sp
	popw	%bx
	leave
4:	# return to 16bit protected mode
	# set CR0 PE bit
	movl	%cr0,	%eax
	andl	$0x7fffffff,%eax
	orl	$0x00000001,%eax
	movl	%eax,	%cr0
	jmp	5f
5:
	# set 32bit protected mode data segment
	movw	$0x0008,%ax
	movw	%ax,	%ss
	movw	%ax,	%ds
	movw	%ax,	%es
	movw	%ax,	%fs
	movw	%ax,	%gs
	# return to 32 bit protected mode
	ljmp	$0x0010,$return_2_32

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

gdtr_16:
	.word	(gdtr_16) - (gdt_16) - 1	# limit of GDT
	.long	gdt_16

gdtr_32:
	.word	0x0000
	.long	0x00000000

esp_32:
	.long	0x00000000

idtr_16:
	.word	0x03ff
	.long	0x00000000

idtr_32:
	.word	0x0000
	.long	0x00000000

ss_32:
	.word	0x0000
ds_32:
	.word	0x0000
es_32:
	.word	0x0000
fs_32:
	.word	0x0000
gs_32:
	.word	0x0000

interrupt_number:
	.byte	0x00
input_ax:
	.word	0x0000
input_cx:
	.word	0x0000
input_bx:
	.word	0x0000
input_dx:
	.word	0x0000
input_si:
	.word	0x0000
input_di:
	.word	0x0000
input_bp:
	.word	0x0000
input_es:
	.word	0x0000
output_ax:
	.word	0x0000
output_cx:
	.word	0x0000
output_bx:
	.word	0x0000
output_dx:
	.word	0x0000
output_flags:
	.word	0x0000
interrupt_number_message:
	.string "interrupt_num = 0x"
input_ax_message:
	.string "input->ax = 0x"
input_cx_message:
	.string "input->cx = 0x"
input_bx_message:
	.string "input->bx = 0x"
input_dx_message:
	.string "input->dx = 0x"
input_si_message:
	.string "input->si = 0x"
input_di_message:
	.string "input->di = 0x"
input_bp_message:
	.string "input->bp = 0x"
input_es_message:
	.string "input->es = 0x"
protected_mode_message:
	.string "PROTECTED MODE NOW!"
real_mode_message:
	.string "REAL MODE NOW!"

