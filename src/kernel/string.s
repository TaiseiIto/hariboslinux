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

	.text

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
1:	# Save the preserved registers.
	pushl	%ebx
	pushl	%edi
	pushl	%esi
2:	# Load the arguments.
	movl	0x08(%ebp),%edi		# EDI = destination;
	movl	0x0c(%ebp),%esi 	# ESI = source;
	movl	0x10(%ebp),%ecx 	# ECX = size;
3:	# Judge the copy direction.
	cmp	%esi,	%edi
	je	7f			# if(destination == source)goto 7;
	jb	5f			# if(destination < source)goto 5;
	movl	%esi,	%edx		# EDX = source;
	addl	%ecx,	%edx		# EDX = source + size;
	cmpl	%edi,	%edx		# if(source + size <= destination)goto 5;
	jbe	5f
4:	# Invert copy direction.
	addl	%ecx,	%esi		# ESI = source + size;
	addl	%ecx,	%edi		# EDI = destination + size;
	testl	%ecx,	%ecx
	jz	7f			# if(!size)goto 7;
	movl	$0x00000001,%eax	# EAX = 0x00000001;
	cmpl	$0x00000002,%ecx	# DL = 2 <= size;
	setae	%dl
	movzx	%dl,	%edx		# EDX = (unsigned int)DL;
	addl	%edx,	%eax		# EAX = 2 <= size ? 2 : 1;
	cmpl	$0x00000004,%ecx	# DL = 4 <= size;
	setae	%dl
	movzx	%dl,	%edx		# EDX = (unsigned int)DL;
	shll	$0x01,	%edx		# EDX = 2 * (4 <= size);
	addl	%edx,	%eax		# EAX = 4 <= size ? 4 : 2 <= size ? 2 : 1;
	subl	%eax,	%esi		# ESI -= EAX;
	subl	%eax,	%edi		# EDI -= EAX;
	std				# DF = 1;
5:	# Copy from source to destination.
	movl	%ecx,	%edx		# EDX = size;
	shrl	$0x02,	%ecx		# ECX = size / 4;
	andl	$0x00000003,%edx	# EDX = size % 4;
	rep	movsl			# while(ECX--)
					# {
					#	*(int *)EDI = *(int *)ESI;
					#	ESI += DF ? -4 : 4;
					#	EDI += DF ? -4 : 4;
					# }
	cmpl	$0x00000002,%edx	# if(size % 4 < 2)goto 6;
	jb	6f
	movsw				# *(short *)EDI = *(short *)ESI;
					# ESI += DF ? -2 : 2;
					# EDI += DF ? -2 : 2;
	subl	$0x00000002,%edx	# EDX = size % 2;
6:	# Copy the last byte.
	jz	7f			# if(!EDX)goto 7;
	movsb				# *(char *)EDI = *(char *)ESI;
					# ESI += DF ? -1 : 1;
					# EDI += DF ? -1 : 1;
7:	# Restore the preserved registers.
	cld
	popl	%esi
	popl	%edi
	popl	%ebx
8:	# End of the function.
	movl	0x08(%ebp),%eax		# return destination;
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
1:	# Save a preserved register.
	pushl	%edi
2:	# Load the arguments.
	movl	0x08(%ebp),%edi		# EDI = buf;
	movb	0x0c(%ebp),%al		# AL = ch;
	movb	%al,	%ah		# AX = {ch, ch};
	movw	%ax,	%dx		# DX = {ch, ch};
	shll	$0x10,	%eax		# EAX = {0, 0, ch, ch};
	movw	%dx,	%ax		# EAX = {ch, ch, ch, ch};
	movl	0x10(%ebp),%ecx		# ECX = size;
3:	# Set buf
	movl	%ecx,	%edx		# EDX = size;
	shrl	$0x02,	%ecx		# ECX = size / 4;
	andl	$0x00000003,%edx	# EDX = size % 4;
	rep	stosl			# while(ECX--)*((int *)EDI)++ = EAX;
	cmpl	$0x00000002,%edx	# if(size % 4 < 2)goto 6;
	jb	4f
	stosw				# *((short *)EDI)++ = AX;
	subl	$0x00000002,%edx	# EDX = size % 2;
4:	# Set the last byte.
	jz	5f			# if(!EDX)goto 7;
	stosb				# *((char *)EDI++) = AL;
5:	# Restore a preserved register.
	pop	%edi
6:	# End of the function.
	movl	0x08(%ebp),%eax		# return buf;
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
1:	# Save a preserved register.
	pushl	%edi
2:	# Load the arguments.
	movl	0x08(%ebp),%edi		# EDI = string;
	pushl	%edi
	call	strlen			# EAX = strlen(string);
	addl	$0x00000004,%esp
	movl	%eax,	%ecx		# ECX = strlen(string);
	movb	0x0c(%ebp),%al		# AL = character;
3:	# Search the character.
	repne	scasb			# while(ECX--)if(*((char *)EDI)++ == AL)break;
	cmpb	%al,	(%edi)		# if(*((char *)EDI != character)goto 4;
	jne	4f
	movl	%edi,	%eax		# EAX = (char *)EDI;
	jmp	5f
4:	# The character is not found.
	xorl	%eax,	%eax		# EAX = NULL;
5:	# Restore a preserved register.
	popl	%edi
6:	# End of the function.
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
1:	# Save preserved registers.
	pushl	%ebx
	pushl	%edi
	pushl	%esi
2:	# Load the arguments.
	movl	0x08(%ebp),%esi		# ESI = string1;
	movl	0x0c(%ebp),%edi		# EDI = string2;
	pushl	%esi
	call	strlen			# EAX = strlen(string1);
	movl	%eax,	%ebx		# EBX = strlen(string1);
	pushl	%edi
	call	strlen			# EAX = strlen(string2);
	addl	$0x00000008,%esp
3:	# Choose shorter string length.
	cmpl	%ebx,	%eax		# if(strlen(string2) < strlen(string1))EAX = strlen(string2);
	jbe	4f			# if(strlen(string1) <= strlen(string2))goto 4;
	movl	%ebx,	%eax		# EAX = strlen(string2);
4:	# Compare the strings.
	movl	%eax,	%ecx		# ECX = min(strlen(string1), strlen(string2));
	repe	cmpsb			# while(ECX--)if(*((char *)ESI)++ != *((char *)EDI)++)break;
	ja	5f			# if(*(char *)ESI < *(char *)EDI)goto 5f;
	jb	6f			# if(*(char *)EDI < *(char *)ESI)
	# if(*(char *)ESI == *(char *)EDI)return 0;
	xorl	%eax,	%eax
	jmp	7f
5:	# if(*(char *)ESI < *(char *)EDI)return -1;
	movl	$0xffffffff,%eax
	jmp	7f
6:	# if(*(char *)EDI < *(char *)ESI)return 1;
	movl	$0x00000001,%eax
7:	# Restore preserved registers.
	popl	%esi
	popl	%edi
	popl	%ebx
8:	# End of the function.
	leave
	ret

# char *strcpy
# (
# 	char *destination,	// 0x08(%ebp)
# 	char const *source	// 0x0c(%ebp)
# );
strcpy:
0:	# Start of the function.
	pushl	%ebp
	movl	%esp,	%ebp
1:	# Get source length.
	pushl	0x0c(%ebp)
	call	strlen
2:	# Call memcpy.
	pushl	%eax
	pushl	0x0c(%ebp)
	pushl	0x08(%ebp)
	call	memcpy
3:	# End of the function.
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
1:	# Save preserved register.
	pushl	%edi
2:	# Load the arguments.
	movl	0x08(%ebp),%edi		# EDI = string;
3:	# Count characters.
	xorl	%eax,	%eax		# EAX = 0;
	movl	$0xffffffff,%ecx	# ECX = -1;
	repne	scasb			# while(ECX--)if(!*((char *)EDI)++)break;
	notl	%ecx			# ECX = strlen(string);
	movl	%ecx,	%eax		# EAX = strlen(string);
4:	# Restore preserved register.
	popl	%edi
5:	# End of the function.
	leave
	ret

