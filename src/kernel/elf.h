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
	unsigned int program_header;
	unsigned int section_header;
	unsigned int flags;
	unsigned short elf_header_size;
	unsigned short program_header_size;
	unsigned short number_of_program_headers;
	unsigned short section_header_size;
	unsigned short number_of_section_headers;
	unsigned short section_names_header_index;
} ELFHeader;

typedef struct _ELFProgramHeader
{
	unsigned int type;
	#define ELF_PROGRAM_HEADER_TYPE_NULL	0x00000000
	#define ELF_PROGRAM_HEADER_TYPE_LOAD	0x00000001
	#define ELF_PROGRAM_HEADER_TYPE_DYNAMIC	0x00000002
	#define ELF_PROGRAM_HEADER_TYPE_INTERP	0x00000003
	#define ELF_PROGRAM_HEADER_TYPE_NOTE	0x00000004
	unsigned int offset_in_file;
	unsigned int deployment_destination;
	unsigned int undefined;
	unsigned int size_in_file;
	unsigned int size_in_memory;
	unsigned int flags;
	#define ELF_PROGRAM_HEADER_FLAG_EXECUTABLE	0x00000001
	#define ELF_PROGRAM_HEADER_FLAG_WRITABLE	0x00000002
	#define ELF_PROGRAM_HEADER_FLAG_READABLE	0x00000004
	unsigned int alignment;
} ELFProgramHeader;

void execute_elf(Shell *shell, ELFHeader const *elf_header);

#endif

