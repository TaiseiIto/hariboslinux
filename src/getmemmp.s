# This program is called from loaddisk.s to get memory map

# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: ax, cx, dx
# preserved registers: bx, si, di, bp, sp

# bibliography
# https://wiki.osdev.org/Detecting_Memory_(x86)

	.include	"global.s"

	.globl	main
	.globl	new_line
	.globl	print
	.globl	putchar

	.type	main,		@function
	.type	new_line,	@function
	.type	print,		@function
	.type	putchar,	@function

	.code16				# real mode
	.text
main:
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	pushw	%si
	pushw	%di
	pushw	%es
	subw	$0x0002,%sp
	movw	%sp,	%di
1:					# print hello message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:
	pushw	%di
	movl	$0x0000e820,%eax
	xorl	%ebx,	%ebx
	movl	$0x00000018,%ecx
	movl	$0x534d4150,%edx
	movw	%bx,	%es
	movw	$0x0900,%di
	int	$0x15
	popw	%di
3:					# free stack frame
	addw	$0x0002,%sp
	popw	%es
	popw	%di
	popw	%si
	popw	%bx
	leave
4:					# jump to initscrn.bin
	hlt
	jmp	3b
	jmp	initscrn

				# // print CRLF
new_line:			# void new_line(void);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	$0x000d,(%di)	# print CR
	call	putchar
	movw	$0x000a,(%di)	# print LF
	call	putchar
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print string to console
print:				# void print(char *string);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%si
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x04(%bp),%si
1:				# put loop
	xorb	%ah,	%ah
	movb	(%si),	%al
	cmpb	$0x0a,	%al
	je	2f		# print CRLF
	cmpb	$0x00,	%al
	je	4f		# finish putting all characters
	movw	%ax,	(%di)
	call	putchar
	jmp	3f
2:				# print CRLF
	movw	$0x000d,(%di)
	call	putchar
	movw	$0x000a,(%di)
	call	putchar
3:
	incw	%si
	jmp	1b		# put next character
4:				# finish putting all characters
	addw	$0x0002,%sp
	popw	%di
	popw	%si
	leave
	ret

				# // print a character to console
putchar:			# void putchar(char c);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%bx
	movb	0x04(%bp),%al
	movb	$0x0e,	%ah
	movw	$0x000f,%bx
	int	$0x10
	popw	%bx
	leave
	ret

	.data
hello_message:
	.string	"Hello, getmemmp.bin!\n"
	.align 0x0200
initscrn:

