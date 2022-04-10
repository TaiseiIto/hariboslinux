#include "elf.h"

char const *elf_header_ability(ELFHeader const *elf_header);
int elf_header_cpu_bits(ELFHeader const *elf_header);
char const *elf_header_endian(ELFHeader const *elf_header);

void execute_elf(Shell *shell, ELFHeader const *elf_header)
{
	printf_shell(shell, "ELF magic number is %s.\n", elf_header->magic_number == ELF_HEADER_MAGIC_NUMBER ? "correct" : "wrong");
	printf_shell(shell, "ELF sign is \"%3s\".\n", elf_header->elf_sign);
	printf_shell(shell, "%d bits CPU\n", elf_header_cpu_bits(elf_header));
	printf_shell(shell, "Endian = %s\n", elf_header_endian(elf_header));
	printf_shell(shell, "ELF header version = %#04.2x\n", elf_header->version);
	printf_shell(shell, "ABI = %#04.2x\n", elf_header->application_binary_interface);
	printf_shell(shell, "Ability = %s\n", elf_header_ability(elf_header));
}

char const *elf_header_ability(ELFHeader const *elf_header)
{
	static char const * const relocatable = "Relocatable";
	static char const * const executable = "Executable";
	static char const * const shared = "Shared";
	static char const * const core = "Core";
	static char const * const invalid = "Invalid";
	switch(elf_header->ability)
	{
	case ELF_HEADER_RELOCATABLE:
		return relocatable;
	case ELF_HEADER_EXECUTABLE:
		return executable;
	case ELF_HEADER_SHARED:
		return shared;
	case ELF_HEADER_CORE:
		return core;
	default:
		return invalid;
	}
}

int elf_header_cpu_bits(ELFHeader const *elf_header)
{
	switch(elf_header->cpu_bits)
	{
	case ELF_HEADER_CPU_32BITS:
		return 32;
	case ELF_HEADER_CPU_64BITS:
		return 64;
	default:
		return -1;
	}
}

char const *elf_header_endian(ELFHeader const *elf_header)
{
	static char const * const little_endian = "Little endian";
	static char const * const big_endian = "Big endian";
	static char const * const invalid = "Invalid";
	switch(elf_header->endian)
	{
	case ELF_HEADER_LITTLE_ENDIAN:
		return little_endian;
	case ELF_HEADER_BIG_ENDIAN:
		return big_endian;
	default:
		return invalid;
	}
}

