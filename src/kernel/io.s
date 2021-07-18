# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.set	whole_segment,		0x08
	.set	kernel_code_segment,	0x10
	.set	kernel_data_segment,	0x18
	.set	vram_segment,		0x20

	.set	memory_size,		0x00007bf4
	.set	vram_addr,		0x00007bf6
	.set	screen_width,		0x00007bfa
	.set	screen_height,		0x00007bfc
	.set	bits_per_pixel,		0x00007bfe
	.set	keyboard_state,		0x00007bff

	.text
	.globl	get_variadic_arg
	.globl	hlt
	.globl	inb
	.globl	inw
	.globl	inl
	.globl	outb
	.globl	outw
	.globl	outl
	.globl	readb
	.globl	readw
	.globl	readl
	.globl	reads
	.globl	writeb
	.globl	writew
	.globl	writel
	.globl	writes
	.type	get_variadic_arg,	@function
	.type	hlt,			@function
	.type	inb,			@function
	.type	inw,			@function
	.type	inl,			@function
	.type	outb,			@function
	.type	outw,			@function
	.type	outl,			@function
	.type	readb,			@function
	.type	readw,			@function
	.type	readl,			@function
	.type	reads,			@function
	.type	writeb,			@function
	.type	writew,			@function
	.type	writel,			@function
	.type	writes,			@function

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

				# // write size bytes from $source_segment:$source to $destination_segment:$destination
reads:				# void writes(unsigned short source_segment, void *source, unsigned short destination_segment, void *destination, unsigned int size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	pushl	%edi
	movw	%es,	%dx
	pushl	%edx
	movw	%fs,	%dx
	pushl	%edx
1:
	movw	0x08(%ebp),%es	# source_segment
	movl	0x0c(%ebp),%esi # source
	movw	0x10(%ebp),%fs	# destination_segment
	movl	0x14(%ebp),%edi # destination
	movl	0x18(%ebp),%ecx # size
2:				# write loop
	jecxz	6f
	cmpl	$0x00000004,%ecx
	jb	4f
3:				# read next 4 bytes
	movl	%es:(%esi),%edx
	movl	%edx,	%fs:(%edi)
	addl	$0x00000004,%esi
	addl	$0x00000004,%edi
	subl	$0x00000004,%ecx
	jmp	2b
4:				# read next 2 bytes
	cmpl	$0x00000002,%ecx
	jb	5f
	movw	%es:(%esi),%dx
	movw	%dx,	%fs:(%edi)
	addl	$0x00000002,%esi
	addl	$0x00000002,%edi
	subl	$0x00000002,%ecx
	jmp	2b
5:				# read a last byte
	movb	%es:(%esi),%dl
	movb	%dl,	%fs:(%edi)
6:				# free stack frame
	popl	%edx
	movw	%dx,	%fs
	popl	%edx
	movw	%dx,	%es
	popl	%edi
	popl	%esi
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
writes:				# void writes(void *source, unsigned short destination_segment, void *destination, unsigned int size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	pushl	%edi
	movw	%es,	%dx
	pushl	%edx
	movw	%fs,	%dx
	pushl	%edx
1:
	movw	0x08(%ebp),%es	# source_segment
	movl	0x0c(%ebp),%esi	# source
	movw	0x10(%ebp),%fs	# destination_segment
	movl	0x14(%ebp),%edi	# destination
	movl	0x18(%ebp),%ecx	# size
2:				# write loop
	jecxz	6f
	cmpl	$0x00000004,%ecx
	jb	4f
3:				# write next 4 bytes
	movl	%es:(%esi),	%edx
	movl	%edx,	%fs:(%edi)
	addl	$0x00000004,%esi
	addl	$0x00000004,%edi
	subl	$0x00000004,%ecx
	jmp	2b
4:				# write next 2 bytes
	cmpl	$0x00000002,%ecx
	jb	5f
	movw	%es:(%esi),	%dx
	movw	%dx,	%fs:(%edi)
	addl	$0x00000002,%esi
	addl	$0x00000002,%edi
	subl	$0x00000002,%ecx
	jmp	2b
5:				# write a last byte
	movb	%es:(%esi),	%dl
	movb	%dl,	%fs:(%edi)
6:				# free stack frame
	popl	%edx
	movw	%dx,	%fs
	popl	%edx
	movw	%dx,	%es
	popl	%edi
	popl	%esi
	leave
	ret

