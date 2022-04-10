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

typedef struct _ELFSectionHeader
{
	unsigned int name_index;
	unsigned int type;
	#define ELF_SECTION_HEADER_TYPE_NULL		0x00000000
	#define ELF_SECTION_HEADER_TYPE_PROGBITS	0x00000001
	#define ELF_SECTION_HEADER_TYPE_SYMTAB		0x00000002
	#define ELF_SECTION_HEADER_TYPE_STRTAB		0x00000003
	#define ELF_SECTION_HEADER_TYPE_RELA		0x00000004
	#define ELF_SECTION_HEADER_TYPE_HASH		0x00000005
	#define ELF_SECTION_HEADER_TYPE_DYNAMIC		0x00000006
	#define ELF_SECTION_HEADER_TYPE_NOTE		0x00000007
	#define ELF_SECTION_HEADER_TYPE_NOBITS		0x00000008
	#define ELF_SECTION_HEADER_TYPE_REL		0x00000009
	#define ELF_SECTION_HEADER_TYPE_SHLIB		0x0000000a
	#define ELF_SECTION_HEADER_TYPE_DYNSYM		0x0000000b
	#define ELF_SECTION_HEADER_TYPE_INIT_ARRAY	0x0000000e
	#define ELF_SECTION_HEADER_TYPE_FINI_ARRAY	0x0000000f
	#define ELF_SECTION_HEADER_TYPE_PREINIT_ARRAY	0x00000010
	#define ELF_SECTION_HEADER_TYPE_GROUP		0x00000011
	#define ELF_SECTION_HEADER_TYPE_SYMTAB_SHNDX	0x00000012
	#define ELF_SECTION_HEADER_TYPE_LOOS		0x60000000
	#define ELF_SECTION_HEADER_TYPE_HIOS		0x6fffffff
	#define ELF_SECTION_HEADER_TYPE_LOPROC		0x70000000
	#define ELF_SECTION_HEADER_TYPE_HIPROC		0x7fffffff
	#define ELF_SECTION_HEADER_TYPE_LOUSER		0x80000000
	#define ELF_SECTION_HEADER_TYPE_HIUSER		0xffffffff
	unsigned int flags;
	#define ELF_SECTION_HEADER_FLAG_WRITE			0x00000001
	#define ELF_SECTION_HEADER_FLAG_ALLOC			0x00000002
	#define ELF_SECTION_HEADER_FLAG_EXECINSTR		0x00000004
	#define ELF_SECTION_HEADER_FLAG_MERGE			0x00000010
	#define ELF_SECTION_HEADER_FLAG_STRINGS			0x00000020
	#define ELF_SECTION_HEADER_FLAG_INFO_LINK		0x00000040
	#define ELF_SECTION_HEADER_FLAG_LINK_ORDER		0x00000080
	#define ELF_SECTION_HEADER_FLAG_OS_NONCONFORMING	0x00000100
	#define ELF_SECTION_HEADER_FLAG_GROUP			0x00000200
	#define ELF_SECTION_HEADER_FLAG_TLS			0x00000400
	#define ELF_SECTION_HEADER_FLAG_MASKOS			0x0ff00000
	#define ELF_SECTION_HEADER_FLAG_MASKPROC		0xf0000000
	unsigned int deployment_destination;
	unsigned int offset_in_file;
	unsigned int size;
	unsigned int linked_section_header_index;
	unsigned int extra_information;
	unsigned int alignment;
	unsigned int entry_size;
} ELFSectionHeader;

void execute_elf(Shell *shell, ELFHeader const *elf_header);

#endif

