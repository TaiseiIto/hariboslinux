# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.globl	call_application
	.globl	cli
	.globl	clts
	.globl	exit_application
	.globl	fninit
	.globl	fnsave
	.globl	frstor
	.globl	get_caller_variadic_arg
	.globl	get_eflags
	.globl	get_variadic_arg
	.globl	hlt
	.globl	inb
	.globl	inw
	.globl	inl
	.globl	lcall
	.globl	lgdt
	.globl	lidt
	.globl	ljmp
	.globl	ltr
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

	.type	call_application,	@function
	.type	cli,			@function
	.type	clts,			@function
	.type	exit_application,	@function
	.type	fninit,			@function
	.type	fnsave,			@function
	.type	frstor,			@function
	.type	get_caller_variadic_arg,@function
	.type	get_eflags,		@function
	.type	get_variadic_arg,	@function
	.type	hlt,			@function
	.type	inb,			@function
	.type	inw,			@function
	.type	inl,			@function
	.type	lcall,			@function
	.type	lgdt,			@function
	.type	lidt,			@function
	.type	ljmp,			@function
	.type	ltr,			@function
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

# unsigned int call_application
# (
# 	unsigned int eip;				// 0x08(%ebp)
# 	unsigned int eflags;				// 0x0c(%ebp)
# 	unsigned int eax;				// 0x10(%ebp)
# 	unsigned int ecx;				// 0x14(%ebp)
# 	unsigned int edx;				// 0x18(%ebp)
# 	unsigned int ebx;				// 0x1c(%ebp)
# 	unsigned int esp;				// 0x20(%ebp)
# 	unsigned int ebp;				// 0x24(%ebp)
# 	unsigned int esi;				// 0x28(%ebp)
# 	unsigned int edi;				// 0x2c(%ebp)
# 	unsigned int es;				// 0x30(%ebp)
# 	unsigned int cs;				// 0x34(%ebp)
# 	unsigned int ss;				// 0x38(%ebp)
# 	unsigned int ds;				// 0x3c(%ebp)
# 	unsigned int fs;				// 0x40(%ebp)
# 	unsigned int gs;				// 0x44(%ebp)
#	void *application_stack_floor;			// 0x48(%ebp)
#	TaskStatusSegment *kernel_task_status_segment;	// 0x4c(%ebp)
# );
call_application:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	# Push kernel registers to kernel stack
	pushal
	pushfl
	# Push kernel segments except cs and ss to kernel stack
	pushl	%es
	pushl	%ds
	pushl	%fs
	pushl	%gs
	# Register kernel stack to kernel_task_status_segment
	movl	0x4c(%ebp),%edi		# edi = kernel_task_status_segment
	movl	%esp,0x04(%edi)
	movw	%ss,0x08(%edi)
	# Push application registers to application stack
	movl	0x48(%ebp),%edi		# edi = application_stack_floor
	movl	0x10(%ebp),%edx		# Push application eax
	movl	%edx,	-0x04(%edi)
	movl	0x14(%ebp),%edx		# Push application ecx
	movl	%edx,	-0x08(%edi)
	movl	0x18(%ebp),%edx		# Push application edx
	movl	%edx,	-0x0c(%edi)
	movl	0x1c(%ebp),%edx		# Push application ebx
	movl	%edx,	-0x10(%edi)
	movl	0x20(%ebp),%edx		# Push application esp
	movl	%edx,	-0x14(%edi)
	movl	0x24(%ebp),%edx		# Push application ebp
	movl	%edx,	-0x18(%edi)
	movl	0x28(%ebp),%edx		# Push application esi
	movl	%edx,	-0x1c(%edi)
	movl	0x2c(%ebp),%edx		# Push application edi
	movl	%edx,	-0x20(%edi)
	movl	0x0c(%ebp),%edx		# Push application eflags
	movl	%edx,	-0x24(%edi)
	# Push application segments except cs and ss to application stack
	movl	0x30(%ebp),%edx		# Push application es
	movl	%edx,	-0x28(%edi)
	movw	%dx,	%es
	movl	0x3c(%ebp),%edx		# Push application ds
	movl	%edx,	-0x2c(%edi)
	movl	0x40(%ebp),%edx		# Push application fs
	movl	%edx,	-0x30(%edi)
	movw	%dx,	%fs
	movl	0x44(%ebp),%edx		# Push application gs
	movl	%edx,	-0x34(%edi)
	movw	%dx,	%gs
	# Return to application
	pushl	0x38(%ebp)		# Push application ss to kernel stack
	pushl	0x20(%ebp)		# Push application esp to kernel stack
	pushl	0x34(%ebp)		# Push application cs to kernel stack
	pushl	0x08(%ebp)		# Push application eip to kernel stack
	lret

				# // disable all interrupts
cli:				# void cli(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	cli	
	leave
	ret

				# // clear task switched flag
clts:				# void clts(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	clts
	leave
	ret

# unsigned int exit_application
# (
# 	unsigned int return_value,	# 0x08(%ebp)
# 	unsigned int esp0		# 0x0c(%ebp)
# );
exit_application:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%eax		# Get application return value
	movl	0x0c(%ebp),%esp		# Switch from application stack to kernel stack
	movl	%eax,	0x30(%esp)	# Store application return value to kernel stack
	popl	%gs			# Restore kernel registers
	popl	%fs
	popl	%ds
	popl	%es
	popfl
	popal
	leave
	ret

				# // initialize FPU
fninit:				# void fninit(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	fninit
	leave
	ret

				# // store FPU registers
fnsave:				# void fnsave(FPURegisters *fpu_registers);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	fnsave	(%edx)
	leave
	ret

				# // load FPU registers
frstor:				# void frstor(FPURegisters const *fpu_registers);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	movl	0x08(%ebp),%edx
	frstor	(%edx)
	leave
	ret

				# // get nth arg in caller variadic arg function
				# // the first arg is 0th
get_caller_variadic_arg:	# unsigned int get_caller_variadic_arg(unsigned int);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	movl	(%ebp),	%esi
	movl	(%esi),	%esi
	movl	0x08(%ebp),%edx
	movl	0x08(%esi,%edx,0x04),%eax
	popl	%esi
	leave
	ret

				# // get eflags register
get_eflags:			# unsigned int get_eflags(void);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushfl
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
	movl	0x08(%esi,%edx,0x04),%eax
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

				# // lcall $segment, %address
lcall:				# void lcall(unsigned int address, unsigned short segment);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	lcall	*0x08(%ebp)
	leave
	ret

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

				# // ljmp $segment, %address
ljmp:				# void ljmp(unsigned int address, unsigned short segment);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	ljmp	*0x08(%ebp)
	leave
	ret

				# // ltr
ltr:				# void ltr(unsigned int tr);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	ltr	0x08(%ebp)
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

