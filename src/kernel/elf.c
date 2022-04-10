#include "elf.h"

void execute_elf(Shell *shell, ELFHeader const *elf)
{
	printf_shell(shell, "ELF magic number %s.\n", elf->magic_number == ELF_HEADER_MAGIC_NUMBER ? "correct" : "wrong");
}

