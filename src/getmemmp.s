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
	.globl	print_byte_hex
	.globl	print_dword_hex
	.globl	print_word_hex
	.globl	putchar

	.type	main,		@function
	.type	new_line,	@function
	.type	print,		@function
	.type	print_byte_hex,	@function
	.type	print_dword_hex,@function
	.type	print_word_hex,	@function
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
	subw	$0x0006,%sp
	movw	%sp,	%di
					# (%di)		argument
					# 0x02(%di)	argument
					# 0x04(%di)	memory region information destination
1:					# print hello message
	call	new_line
	movw	$hello_message,(%di)
	call	print
2:
	movw	$0x0900,0x04(%di)	# start point of memory map
	pushw	%di
	movl	$0x0000e820,%eax
	xorl	%ebx,	%ebx
	movl	$0x00000018,%ecx
	movl	$0x534d4150,%edx
	movw	%bx,	%es
	movw	0x04(%di),%di
	int	$0x15
	popw	%di
	movw	$base_address_message,(%di)	# print base address
	call	print
	movw	0x04(%di),%si
	movw	(%si),	%dx
	movw	%dx,	(%di)
	movw	0x02(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex
	movw	$length_message,(%di)	# print length
	call	print
	movw	0x04(%si),%dx
	movw	%dx,	(%di)
	movw	0x06(%si),%dx
	movw	%dx,	0x02(%di)
	call	print_dword_hex
3:					# free stack frame
	addw	$0x0006,%sp
	popw	%es
	popw	%di
	popw	%si
	popw	%bx
	leave
4:					# jump to initscrn.bin
	hlt
	jmp	4b
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

				# // print value as hexadecimal
print_byte_hex:			# void print_byte_hex(unsigned value);
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
	call	putchar
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

print_dword_hex:		# void print_dword_hex(unsigned low, unsigned short high);
0:
	pushw	%bp
	movw	%sp,	%bp
	pushw	%di
	subw	$0x0002,%sp
	movw	%sp,	%di
	movw	0x06(%bp),%dx
	movw	%dx,	(%di)
	call	print_word_hex
	movw	0x04(%bp),%dx
	movw	%dx,	(%di)
	call	print_word_hex
	addw	$0x0002,%sp
	popw	%di
	leave
	ret

				# // print value as hexadecimal
print_word_hex:			# void print_word_hex(unsigned short value);
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
	call	print_byte_hex
	movw	0x02(%di),%dx
	andw	$0x00ff,%dx
	movw	%dx,	(%di)
	call	print_byte_hex
	addw	$0x0004,%sp
	popw	%di
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
base_address_message:
	.string "Base Address = 0x"
hello_message:
	.string	"Hello, getmemmp.bin!\n\n"
length_message:
	.string ", Length = 0x"
	.align 0x0200
initscrn:

