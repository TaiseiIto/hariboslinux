#ifndef _ELF_H_
#define _ELF_H_

// Bibliography
// https://wiki.osdef.org/ELF/

#include "shell.h"

typedef struct _ELFHeader
{
	// The magic number must be 0x7f
	unsigned char magic_number;
	#define ELF_HEADER_MAGIC_NUMBER	0x7f
	// The ELF sign must be "ELF"
	char elf_sign[3];
	unsigned char cpu_bit;
	#define ELF_HEADER_CPU_32BIT	0x01
	#define ELF_HEADER_CPU_64BIT	0x02
	unsigned char endian;
	#define ELF_HEADER_LITTLE_ENDIAN	0x01
	#define ELF_HEADER_BIG_ENDIAN		0x02
	unsigned char version;
	unsigned char application_binary_interface;
	unsigned long long unused;
	unsigned short ability;
} ELFHeader;

void execute_elf(Shell *shell, ELFHeader const *elf);

#endif

