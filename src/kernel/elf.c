#include "elf.h"

void execute_elf(Shell *shell, ELFHeader const *elf_header)
{
	printf_shell(shell, "ELF magic number is %s.\n", elf_header->magic_number == ELF_HEADER_MAGIC_NUMBER ? "correct" : "wrong");
	printf_shell(shell, "ELF sign is \"%3s\".\n", elf_header->elf_sign);
	printf_shell(shell, "%d bits CPU\n", 32 * elf_header->cpu_bit);
	printf_shell(shell, "%s endian\n", elf_header->endian == ELF_HEADER_LITTLE_ENDIAN ? "Little" : "Big");
	printf_shell(shell, "ELF header version = %#04.2x\n", elf_header->version);
	printf_shell(shell, "ABI = %#04.2x\n", elf_header->application_binary_interface);
	printf_shell(shell, "Ability = %#06.4x\n", elf_header->ability);
}

