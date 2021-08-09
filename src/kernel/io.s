# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.extern	bound_range_exceeded_exception_handler
	.extern	device_not_available_exception_handler
	.extern	devide_by_zero_exception_handler
	.extern double_fault_exception_handler
	.extern	invalid_opcode_exception_handler
	.extern	keyboard_interrupt_handler

	.globl	cli
	.globl	get_eflags
	.globl	get_variadic_arg
	.globl	hlt
	.globl	inb
	.globl	inw
	.globl	inl
	.globl	interrupt_handler0x00
	.globl	interrupt_handler0x05
	.globl	interrupt_handler0x06
	.globl	interrupt_handler0x07
	.globl	interrupt_handler0x08
	.globl	interrupt_handler0x21
	.globl	lgdt
	.globl	lidt
	.globl	outb
	.globl	outw
	.globl	outl
	.globl	readb
	.globl	readw
	.globl	readl
	.globl	reads
	.globl	sti
	.globl	writeb
	.globl	writew
	.globl	writel
	.globl	writes

	.type	cli,			@function
	.type	get_eflags,		@function
	.type	get_variadic_arg,	@function
	.type	hlt,			@function
	.type	inb,			@function
	.type	inw,			@function
	.type	inl,			@function
	.type	interrupt_handler0x00,	@function
	.type	interrupt_handler0x05,	@function
	.type	interrupt_handler0x06,	@function
	.type	interrupt_handler0x07,	@function
	.type	interrupt_handler0x08,	@function
	.type	interrupt_handler0x21,	@function
	.type	lgdt,			@function
	.type	lidt,			@function
	.type	outb,			@function
	.type	outw,			@function
	.type	outl,			@function
	.type	readb,			@function
	.type	readw,			@function
	.type	readl,			@function
	.type	reads,			@function
	.type	sti,			@function
	.type	writeb,			@function
	.type	writew,			@function
	.type	writel,			@function
	.type	writes,			@function

	.text

				# // disable all interrupts
cli:				# void cli(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	cli	
	leave
	ret

				# // get eflags register
get_eflags:			# unsigned int get_eflags(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushf
	popl	%eax
	leave
	ret

				# // get nth arg in variadic arg function
				# // the first arg is 0th
get_variadic_arg:		# unsigned int get_variadic_arg(unsigned int n);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	movl	(%ebp),	%esi
	movl	0x08(%ebp),%edx
	movl	%ss:0x08(%esi,%edx,0x04),%eax
	popl	%esi
	leave
	ret

				# // wait for next interrupt
hlt:				# void io_hlt(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	hlt
	leave
	ret

				# // inb	$address,	%al
inb:				# unsigned char io_inb(unsigned short address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	inb	%dx,	%al
	leave
	ret

				# // inw	$address,	%ax
inw:				# unsigned short io_inw(unsigned short address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	inw	%dx,	%ax
	leave
	ret

				# // inl	$address,	%eax
inl:				# unsigned int io_inl(unsigned short address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	inl	%dx,	%eax
	leave
	ret

				# // devide by 0 exception handler
interrupt_handler0x00:		# void interrupt_handler0x00(void);
0:
	pushal
	call	devide_by_zero_exception_handler
	popal
	iret

				# // bound range exceeded exception handler
interrupt_handler0x05:		# void interrupt_handler0x05(void);
0:
	pushal
	call	bound_range_exceeded_exception_handler
	popal
	iret

				# // invalid opcode exception handler
interrupt_handler0x06:		# void interrupt_handler0x06(void);
0:
	pushal
	call	invalid_opcode_exception_handler
	popal
	iret

				# // device not available exception handler
interrupt_handler0x07:		# void interrupt_handler0x07(void);
0:
	pushal
	call	device_not_available_exception_handler
	popal
	iret

				# // double fault exception handler
interrupt_handler0x08:		# void interrupt_handler0x08(void);
0:
	pushal
	call	double_fault_exception_handler
	popal
	iret

				# // kerboard interrupt handler
interrupt_handler0x21:		# void interrupt_handler0x21(void);
0:
	pushal
	call	keyboard_interrupt_handler
	popal
	iret

				# // lgdt
lgdt:				# void lgdt(unsigned int limit, SegmentDescriptor *begin);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	shll	$0x10,	0x08(%ebp)
	lgdt	0x0a(%ebp)
	leave
	ret

				# // lidt
lidt:				# void lidt(unsigned int limit, SegmentDescriptor *begin);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	shll	$0x10,	0x08(%ebp)
	lidt	0x0a(%ebp)
	leave
	ret

				# // outb	$value,	$address
outb:				# void io_outb(unsigned short address, unsigned char value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	movb	0x0c(%ebp),%al
	outb	%al,	%dx
	leave
	ret
 
				# // outw	$value,	$address
outw:				# void io_outw(unsigned short address, unsigned char value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	movw	0x0c(%ebp),%ax
	outw	%ax,	%dx
	leave
	ret
 
				# // outl	$value,	$address
outl:				# void io_outl(unsigned short address, unsigned char value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	movl	0x0c(%ebp),%eax
	outl	%eax,	%dx
	leave
	ret

				# // movb	$segment:($address),%al
readb:				# unsigned char io_readb(unsigned short segment, void *address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	movw	%es,	%dx	# preserve %es
	pushl	%edx
	movw	0x08(%ebp),%es
	movl	0x0c(%ebp),%esi
	movb	%es:(%esi),%al
	popl	%edx
	movw	%dx,	%es	# restore %es
	popl	%esi
	leave
	ret

				# // movw	$segment:($address),%ax
readw:				# unsigned short io_readw(unsigned short segment, void *address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	movw	%es,	%dx	# preserve %es
	pushl	%edx
	movw	0x08(%ebp),%es
	movl	0x0c(%ebp),%esi
	movw	%es:(%esi),%ax
	popl	%edx
	movw	%dx,	%es	# restore %es
	popl	%esi
	leave
	ret

				# // movl	$segment:($address),%eax
readl:				# unsigned int io_readl(unsigned short segment, void *address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	movw	%es,	%dx	# preserve %es
	pushl	%edx
	movw	0x08(%ebp),%es
	movl	0x0c(%ebp),%esi
	movl	%es:(%esi),%eax
	popl	%edx
	movw	%dx,	%es	# restore %es
	popl	%esi
	leave
	ret

				# // read size bytes from $source_segment:$source to $destination_segment:$destination
reads:				# void read(unsigned short source_segment, void const *source, void *destination, unsigned int size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	pushl	%edi
	movw	%es,	%dx
	pushl	%edx
1:
	movw	0x08(%ebp),%es	# source_segment
	movl	0x0c(%ebp),%esi # source
	movl	0x10(%ebp),%edi # destination
	movl	0x14(%ebp),%ecx # size
2:				# write loop
	jecxz	6f
	cmpl	$0x00000004,%ecx
	jb	4f
3:				# read next 4 bytes
	movl	%es:(%esi),%edx
	movl	%edx,	(%edi)
	addl	$0x00000004,%esi
	addl	$0x00000004,%edi
	subl	$0x00000004,%ecx
	jmp	2b
4:				# read next 2 bytes
	cmpl	$0x00000002,%ecx
	jb	5f
	movw	%es:(%esi),%dx
	movw	%dx,	(%edi)
	addl	$0x00000002,%esi
	addl	$0x00000002,%edi
	subl	$0x00000002,%ecx
	jmp	2b
5:				# read a last byte
	movb	%es:(%esi),%dl
	movb	%dl,	(%edi)
6:				# free stack frame
	popl	%edx
	movw	%dx,	%es
	popl	%edi
	popl	%esi
	leave
	ret

				# // enable all interrupts
sti:				# void sti(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	sti	
	leave
	ret

				# // movb	$value,$segment:($address)
writeb:				# void io_writeb(unsigned short segment, void *address, unsigned char value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%edi
	movw	%es,	%dx	# preserve %es
	pushl	%edx
	movw	0x08(%ebp),%es
	movl	0x0c(%ebp),%edi
	movb	0x10(%ebp),%dl
	movb	%dl,	%es:(%edi)
	popl	%edx
	movw	%dx,	%es	# restore %es
	popl	%edi
	leave
	ret

				# // movw	$value,$segment:($address)
writew:				# void io_writew(unsigned short segment, void *address, unsigned char value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%edi
	movw	%es,	%dx	# preserve %es
	pushl	%edx
	movw	0x08(%ebp),%es
	movl	0x0c(%ebp),%edi
	movw	0x10(%ebp),%dx
	movw	%dx,	%es:(%edi)
	popl	%edx
	movw	%dx,	%es	# restore %es
	popl	%edi
	leave
	ret

				# // movl	$value,$segment:($address)
writel:				# void io_writel(unsigned short segment, void *address, unsigned char value);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%edi
	movw	%es,	%dx	# preserve %es
	pushl	%edx
	movw	0x08(%ebp),%es
	movl	0x0c(%ebp),%edi
	movl	0x10(%ebp),%edx
	movl	%edx,	%es:(%edi)
	popl	%edx
	movw	%dx,	%es	# restore %es
	popl	%edi
	leave
	ret

				# // write size bytes from $source to $destination_segment:$destination
writes:				# void writes(void const *source, unsigned short destination_segment, void *destination, unsigned int size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	pushl	%edi
	movw	%es,	%dx
	pushl	%edx
1:
	movl	0x08(%ebp),%esi	# source
	movw	0x0c(%ebp),%es	# destination_segment
	movl	0x10(%ebp),%edi	# destination
	movl	0x14(%ebp),%ecx	# size
2:				# write loop
	jecxz	6f
	cmpl	$0x00000004,%ecx
	jb	4f
3:				# write next 4 bytes
	movl	(%esi),	%edx
	movl	%edx,	%es:(%edi)
	addl	$0x00000004,%esi
	addl	$0x00000004,%edi
	subl	$0x00000004,%ecx
	jmp	2b
4:				# write next 2 bytes
	cmpl	$0x00000002,%ecx
	jb	5f
	movw	(%esi),	%dx
	movw	%dx,	%es:(%edi)
	addl	$0x00000002,%esi
	addl	$0x00000002,%edi
	subl	$0x00000002,%ecx
	jmp	2b
5:				# write a last byte
	movb	(%esi),	%dl
	movb	%dl,	%es:(%edi)
6:				# free stack frame
	popl	%edx
	movw	%dx,	%es
	popl	%edi
	popl	%esi
	leave
	ret

