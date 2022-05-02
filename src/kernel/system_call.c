#include "common.h"
#include "io.h"
#include "shell.h"
#include "system_call.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

unsigned int system_call(unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi, unsigned int ebp)
{
	UNUSED_ARGUMENT(ecx);
	UNUSED_ARGUMENT(edx);
	UNUSED_ARGUMENT(esi);
	UNUSED_ARGUMENT(edi);
	UNUSED_ARGUMENT(ebp);
	#define SYSTEM_CALL_EXIT	0x00000001
	#define SYSTEM_CALL_WRITE	0x00000004
	switch(eax)
	{
	case SYSTEM_CALL_EXIT:
		exit_application(ebx, get_current_task()->task_status_segment.esp0);
		break;
	case SYSTEM_CALL_WRITE:
		system_call_write(ebx, (void const *)ecx, (size_t)edx);
		break;
	default:
		ERROR(); // Invalid eax
		break;
	}
	return 0;
}

unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count)
{
	Shell *shell = get_current_shell();
	unsigned int counter = 0;
	switch(file_descriptor)
	{
	case STDOUT:
	case STDERR:
		for(void const *reader = buffer; reader != buffer + count && *(char const *)reader; reader++)
		{
			put_char_shell(shell, *(char const *)reader);
			counter++;
		}
		break;
	default:
		ERROR(); // Invalid file descriptor
		break;
	}
	return counter;
}

