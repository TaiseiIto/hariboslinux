# This program is called from mv2prtmd.s to deploy disk image to 0x00100000 and jump to kernel
#
# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

	.include	"global.s"

	.globl	main
	.globl	memcpy
	.globl	new_line_serial
	.globl	print_serial
	.globl	putchar_serial

	.type	main,		@function
	.type	memcpy,		@function
	.type	new_line_serial,@function
	.type	print_serial,	@function
	.type	putchar_serial,	@function

	.code32
	.text
main:
0:
	pushl	%ebp
	movl	%esp,	%ebp
	subl	$0x00000014,%esp
1:					# hello message
	call	new_line_serial
	movl	$hello_message,(%esp)
	call	print_serial
2:					# deploy disk image to 0x00100000
	movl	$0x00000008,(%esp)
	movl	$0x00100000,0x04(%esp)
	movl	$0x00000008,0x08(%esp)
	movl	$0x00007c00,0x0c(%esp)
	movl	$0x00168000,0x10(%esp)
	call	memcpy
3:					# finish message
	movl	$finish_message,(%esp)
	call	print_serial
4:					# free stack frame
	addl	$0x00000014,%esp
	leave
5:					# jump to kernel
	jmp	lddskxtr

memcpy:				# void memcpy(unsigned short dest_seg, void *dest_addr, unsigned short src_seg, void *src_addr, unsigned short size);
0:
	pushl	%ebp
	movl	%esp,	%ebp
	pushl	%esi
	pushl	%edi
	movw	%es,	%dx
	shll	$0x10,	%edx
	movw	%fs,	%dx
	pushl	%edx
1:
	movw	0x08(%ebp),%fs	# dest_seg
	movl	0x0c(%ebp),%edi	# dest_addr
	movw	0x10(%ebp),%es	# src_seg
	movl	0x14(%ebp),%esi	# src_addr
	movl	0x18(%ebp),%ecx	# size
2:
	jecxz	6f
	cmpl	$0x00000004,%ecx
	jb	4f
3:
	movl	%es:(%esi),%edx
	movl	%edx,	%fs:(%edi)
	addl	$0x00000004,%esi
	addl	$0x00000004,%edi
	subl	$0x00000004,%ecx
	jmp	2b
4:
	cmpl	$0x00000002,%ecx
	jb	5f
	movw	%es:(%esi),%dx
	movw	%dx,	%fs:(%edi)
	addl	$0x00000002,%esi
	addl	$0x00000002,%edi
	subl	$0x00000002,%ecx
	jmp	2b
5:
	movb	%es:(%esi),%dl
	movb	%dl,	%fs:(%edi)
6:
	popl	%edx
	movw	%dx,	%fs
	shrl	$0x10,	%edx
	movw	%dx,	%es
	popl	%edi
	popl	%esi
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
finish_message:
	.string "finish deploying the disk image!\n"
hello_message:
	.string "Hello, dplydisk.bin!\n\n"
	.align	0x0200
lddskxtr:

