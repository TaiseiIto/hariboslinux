# This program is called from initscrn.s to move to protected mode

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
	.globl	send_byte_to_keyboard
	.globl	wait_for_keyboard

	.type	main,			@function
	.type	new_line_serial,	@function
	.type	print_serial,		@function
	.type	putchar_serial,		@function
	.type	send_byte_to_keyboard,	@function
	.type	wait_for_keyboard,	@function

	.code16				# real mode
	.text
main:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0004,%sp
	movw	%sp,	%di
1:					# print hello message
	call	new_line_serial
	movw	$hello_message,(%di)
	call	print_serial
2:					# prepare to move to protected mode
	movw	$0x00ff,%ax		# disable master PIC
	movw	$0x0021,%dx
	outb	%al,	%dx
	call	new_line_serial
	movw	$disable_master_PIC_message,(%di)
	call	print_serial
	movw	$0x00ff,%ax		# disable slave PIC
	movw	$0x00a1,%dx
	outb	%al,	%dx
	movw	$disable_slave_PIC_message,(%di)
	call	print_serial
	cli				# disable interrupts
	movw	$disable_interrupts_message,(%di)
	call	print_serial
	movw	$0x0064,(%di)		# enable memory space beyond 0xf:ffff
	movw	$0x00d1,0x02(%di)
	call	send_byte_to_keyboard
	movw	$0x0060,(%di)
	movw	$0x00df,0x02(%di)
	call	send_byte_to_keyboard
	call	wait_for_keyboard
	movw	$expand_memory_message,(%di)
	call	print_serial
3:					# free stack frame
	addw	$0x0004,%sp
	popw	%di
	popw	%si
	leave
	pushw	$free_stack_message
	call	print_serial
	addw	$0x0002,%sp
4:					# move to protected mode
	lgdt	(gdtr)
	pushw	$lgdt_message
	call	print_serial
	addw	$0x0002,%sp
	movl	%cr0,	%eax
	andl	$0x7fffffff,%eax
	orl	$0x00000001,%eax
	movl	%eax,	%cr0
	movw	$0x0008,%dx
	movw	%dx,	%ds
	movw	%dx,	%es
	movw	%dx,	%fs
	movw	%dx,	%gs
	movw	%dx,	%ss
	movl	$0x00007c00,%ebp
	movl	$0x00007c00,%esp
	jmp	$0x0010,$dplydisk

				# // print LF
new_line_serial:		# void new_line_serial(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000a,(%di)	# print LF
	call	putchar_serial
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print string to serial port COM1
print_serial:			# void print_serial(char *string);
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
	call	putchar_serial
	incw	%si
	jmp	1b		# put next character
2:				# finish putting all characters
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print a character to console
putchar_serial:			# void putchar_serial(char c);
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
3:				# free stack frame
	leave
	ret

send_byte_to_keyboard:		# void send_byte_to_keyboard(unsigned short port, unsigned char data);
0:
	pushw	%bp
	movw	%sp,	%bp
	call	wait_for_keyboard
	movw	0x04(%bp),%dx
	movb	0x06(%bp),%al
	outb	%al,	%dx
	leave
	ret

wait_for_keyboard:		# void wait_for_keyboard(void);
0:
	pushw	%bp
	movw	%sp,	%bp
1:
	movw	$0x0064,%dx
	inb	%dx,	%al
	andb	$0x02,	%al
	jnz	1b
	leave
	ret

	.data
	.align	0x8
gdt:
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
gdtr:
	.word	(gdtr) - (gdt) - 1	# limit of GDT
	.long	gdt
disable_interrupts_message:
	.string "disable interrupts\n"
disable_master_PIC_message:
	.string "disable master PIC\n"
disable_slave_PIC_message:
	.string "disable slave PIC\n"
expand_memory_message:
	.string "expand memory beyond 0xf:ffff\n"
free_stack_message:
	.string "free stack!\n"
hello_message:
	.string	"Hello, mv2prtmd.bin!\n"
lgdt_message:
	.string "lgdt!\n"
	.align	0x0200
dplydisk:

