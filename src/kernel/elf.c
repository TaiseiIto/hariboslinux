#include "elf.h"

char const *elf_header_ability(ELFHeader const *elf_header);
int elf_header_cpu_bits(ELFHeader const *elf_header);
char const *elf_header_endian(ELFHeader const *elf_header);
char const *elf_header_instruction_set(ELFHeader const *elf_header);

void execute_elf(Shell *shell, ELFHeader const *elf_header)
{
	printf_shell(shell, "ELF magic number is %s.\n", elf_header->magic_number == ELF_HEADER_MAGIC_NUMBER ? "correct" : "wrong");
	printf_shell(shell, "ELF sign is \"%.3s\".\n", elf_header->elf_sign);
	printf_shell(shell, "%d bits CPU\n", elf_header_cpu_bits(elf_header));
	printf_shell(shell, "Endian = %s\n", elf_header_endian(elf_header));
	printf_shell(shell, "ELF header version = %#04.2x\n", elf_header->elf_header_version);
	printf_shell(shell, "ABI = %#04.2x\n", elf_header->application_binary_interface);
	printf_shell(shell, "Ability = %s\n", elf_header_ability(elf_header));
	printf_shell(shell, "Instruction set = %s\n", elf_header_instruction_set(elf_header));
	printf_shell(shell, "ELF version = %#010.8x\n", elf_header->elf_version);
	printf_shell(shell, "Entry point = %#010.8x\n", elf_header->entry_point);
	printf_shell(shell, "Program header table = %#010.8x\n", elf_header->program_header_table);
	printf_shell(shell, "Section header table = %#010.8x\n", elf_header->section_header_table);
	printf_shell(shell, "Flags = %#010.8x\n", elf_header->flags);
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

char const *elf_header_instruction_set(ELFHeader const *elf_header)
{
	static char const * const no_specific = "No specific";
	static char const * const spark = "Spark";
	static char const * const x86 = "x86";
	static char const * const mips = "MIPS";
	static char const * const powerpc = "PowerPC";
	static char const * const arm = "ARM";
	static char const * const superh = "SuperH";
	static char const * const ia64 = "IA-64";
	static char const * const x86_64 = "x86-64";
	static char const * const aarch64 = "AARCH64";
	static char const * const risc_v = "RISC-V";
	static char const * const invalid = "Invalid";
	switch(elf_header->instruction_set)
	{
	case ELF_HEADER_INSTRUCTION_SET_NO_SPECIFIC:
		return no_specific ;
	case ELF_HEADER_INSTRUCTION_SET_SPARK:
		return spark;
	case ELF_HEADER_INSTRUCTION_X86:
		return x86;
	case ELF_HEADER_INSTRUCTION_MIPS:
		return mips;
	case ELF_HEADER_INSTRUCTION_POWERPC:
		return powerpc;
	case ELF_HEADER_INSTRUCTION_ARM:
		return arm;
	case ELF_HEADER_INSTRUCTION_SUPERH:
		return superh;
	case ELF_HEADER_INSTRUCTION_IA64:
		return ia64;
	case ELF_HEADER_INSTRUCTION_X86_64:
		return x86_64;
	case ELF_HEADER_INSTRUCTION_AARCH64:
		return aarch64;
	case ELF_HEADER_INSTRUCTION_RISC_V:
		return risc_v;
	default:
		return invalid;
	}
}

