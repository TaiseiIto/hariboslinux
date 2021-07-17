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
	.globl	io_hlt
	.globl	io_inb
	.globl	io_inw
	.globl	io_inl
	.globl	io_outb
	.globl	io_outw
	.globl	io_outl
	.globl	write_pixel
	.type	io_hlt,			@function
	.type	io_inb,			@function
	.type	io_inw,			@function
	.type	io_inl,			@function
	.type	io_outb,		@function
	.type	io_outw,		@function
	.type	io_outl,		@function
	.type	write_pixel,	@function

				# // wait for next interrupt
io_hlt:				# void io_hlt(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	hlt
	leave
	ret

				# // inb	$address,	%al
io_inb:				# unsigned char io_inb(unsigned short address);
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
io_inw:				# unsigned short io_inw(unsigned short address);
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
io_inl:				# unsigned int io_inl(unsigned short address);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	xorl	%eax,	%eax
	xorl	%edx,	%edx
	movw	0x08(%ebp),%dx
	inl	%dx,	%eax
	leave
	ret

				# // outb	%al,	$address
io_outb:			# void io_outb(unsigned short address, unsigned char value);
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
 
				# // outw	%ax,	$address
io_outw:			# void io_outw(unsigned short address, unsigned char value);
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
 
				# // outl	%eax,	$address
io_outl:			# void io_outl(unsigned short address, unsigned char value);
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

