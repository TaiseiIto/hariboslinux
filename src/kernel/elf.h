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
	unsigned char cpu_bits;
	#define ELF_HEADER_CPU_32BITS	0x01
	#define ELF_HEADER_CPU_64BITS	0x02
	unsigned char endian;
	#define ELF_HEADER_LITTLE_ENDIAN	0x01
	#define ELF_HEADER_BIG_ENDIAN		0x02
	unsigned char elf_header_version;
	unsigned char application_binary_interface;
	unsigned long long unused;
	unsigned short ability;
	#define ELF_HEADER_RELOCATABLE	0x01
	#define ELF_HEADER_EXECUTABLE	0x02
	#define ELF_HEADER_SHARED	0x03
	#define ELF_HEADER_CORE		0x04
	unsigned short instruction_set;
	#define ELF_HEADER_INSTRUCTION_SET_NO_SPECIFIC	0x0000
	#define ELF_HEADER_INSTRUCTION_SET_SPARK	0x0002
	#define ELF_HEADER_INSTRUCTION_X86		0x0003
	#define ELF_HEADER_INSTRUCTION_MIPS		0x0008
	#define ELF_HEADER_INSTRUCTION_POWERPC		0x0014
	#define ELF_HEADER_INSTRUCTION_ARM		0x0028
	#define ELF_HEADER_INSTRUCTION_SUPERH		0x002a
	#define ELF_HEADER_INSTRUCTION_IA64		0x0032
	#define ELF_HEADER_INSTRUCTION_X86_64		0x003e
	#define ELF_HEADER_INSTRUCTION_AARCH64		0x00b7
	#define ELF_HEADER_INSTRUCTION_RISC_V		0x00f3
	unsigned int elf_version;
	unsigned int entry_point;
	unsigned int program_header_table;
	unsigned int section_header_table;
	unsigned int flags;
} ELFHeader;

void execute_elf(Shell *shell, ELFHeader const *elf_header);

#endif

