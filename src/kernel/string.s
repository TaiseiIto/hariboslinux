# calling convention = System V i386
# return value: ax, dx
# parameters: stack
# scratch registers: eax, ecx, edx
# preserved registers: ebx, esi, edi, ebp, esp

	.globl	memcpy
	.globl	memset
	.globl	strchr
	.globl	strcmp
	.globl	strcpy
	.globl	strlen

	.type	memcpy,	@function
	.type	memset,	@function
	.type	strchr,	@function
	.type	strcmp,	@function
	.type	strcpy,	@function
	.type	strlen,	@function

# void *memcpy
# (
#	void *destination,	// 0x08(%ebp)
#	void const *source,	// 0x0c(%ebp)
#	size_t size		// 0x10(%ebp)
# );
memcpy:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# Save the scratch registers.
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushfl
2:	# Get arguments.
	movl	0x08(%ebp),%edi		# edi = destination;
	movl	0x0c(%ebp),%esi 	# esi = source;
	movl	0x10(%ebp),%ecx 	# ecx = size;
3:	# Judge the copy direction.
	cmp	%esi,	%edi
	je	7f			# if(destination == source)goto 7;
	jb	5f			# if(destination < source)goto 5;
	movl	%esi,	%edx		# edx = source;
	addl	%ecx,	%edx		# edx = source + size;
	cmp	%edi,	%edx		# if(source + size <= destination)goto 5;
	jbe	5f
4:	# Invert copy direction.
	addl	%ecx,	%esi		# esi = source + size;
	addl	%ecx,	%edi		# edi = destination + size;
	test	%ecx,	%ecx
	jz	7f			# if(!size)goto 7;
	movl	$0x00000001,%eax	# eax = 0x00000001;
	cmp	$0x00000002,%ecx	# dl = 2 <= size;
	setae	%dl
	movzx	%dl,	%edx		# edx = (unsigned int)dl;
	addl	%edx,	%eax		# eax = 2 <= size ? 2 : 1;
	cmp	$0x00000004,%ecx	# dl = 4 <= size;
	setae	%dl
	movzx	%dl,	%edx		# edx = (unsigned int)dl;
	shll	$0x01,	%edx		# edx = 2 * (4 <= size);
	addl	%edx,	%eax		# eax = 4 <= size ? 4 : 2 <= size ? 2 : 1;
	subl	%eax,	%esi		# esi -= eax;
	subl	%eax,	%edi		# edi -= eax;
	std				# DF = 1;
5:	# Copy from source to destination.
	movl	%ecx,	%eax		# eax = size;
	xorl	%edx,	%edx		# edx = 0;
	movl	$0x00000004,%ebx	# ebx = 0x00000004;
	divl	%ebx			# eax = size / 4;
					# edx = size % 4;
	movl	%eax,	%ecx		# ecx = size / 4;
	rep	movsl			# while(ecx--)
					# {
					#	*(int *)edi = *(int *)esi;
					#	esi += DF ? -4 : 4;
					#	edi += DF ? -4 : 4;
					# }
	cmpl	$0x00000002,%edx	# if(size % 4 < 2)goto 6;
	jb	6f
	movsw				# *(short *)edi = *(short *)esi;
					# esi += DF ? -2 : 2;
					# edi += DF ? -2 : 2;
	subl	$0x00000002,%edx
6:	# Copy the last byte.
	jz	7f			# if(!edx)goto 7;
	movsb				# *(char *)edi = *(char *)esi;
					# esi += DF ? -1 : 1;
					# edi += DF ? -1 : 1;
7:	# Restore the scratch registers.
	popfl
	popl	%edi
	popl	%esi
	popl	%ebx
8:	# End of the function.
	leave
	ret

# void *memset
# (
# 	void *buf,		// 0x08(%ebp)
# 	char ch,		// 0x0c(%ebp)
# 	size_t size		// 0x10(%ebp)
# );
memset:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# End of the function.
	leave
	ret

# char const *strchr
# (
# 	char const *string,	// 0x08(%ebp)
# 	char character		// 0x0c(%ebp)
# );
strchr:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# End of the function.
	leave
	ret

# int strcmp
# (
# 	char const *string1,	// 0x08(%ebp)
# 	char const *string2	// 0x0c(%ebp)
# );
strcmp:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# End of the function.
	leave
	ret

# char *strcpy
# (
# 	char *destination,	// 0x08(%ebp)
# 	char const *source	// 0x08(%ebp)
# );
strcpy:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# End of the function.
	leave
	ret

# unsigned int strlen
# (
# 	char const *string	// 0x08(%ebp)
# );
strlen:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# End of the function.
	leave
	ret

