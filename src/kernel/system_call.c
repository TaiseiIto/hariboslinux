#include "common.h"
#include "event.h"
#include "io.h"
#include "shell.h"
#include "system_call.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

unsigned int system_call_exit(unsigned int return_value);
unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

unsigned int system_call(unsigned int eax, unsigned int ebx, unsigned int ecx, unsigned int edx, unsigned int esi, unsigned int edi, unsigned int ebp)
{
	#define SYSTEM_CALL_EXIT	0x00000001
	#define SYSTEM_CALL_WRITE	0x00000004
	printf_serial("System call arguments\n");
	printf_serial("eax = %#010.8x\n", eax);
	printf_serial("ebx = %#010.8x\n", ebx);
	printf_serial("ecx = %#010.8x\n", ecx);
	printf_serial("edx = %#010.8x\n", edx);
	printf_serial("esi = %#010.8x\n", esi);
	printf_serial("edi = %#010.8x\n", edi);
	printf_serial("ebp = %#010.8x\n", ebp);
	switch(eax)
	{
	case SYSTEM_CALL_EXIT:
		return system_call_exit(ebx);
	case SYSTEM_CALL_WRITE:
		return system_call_write(ebx, (void const *)(ecx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (size_t)edx);
	default:
		ERROR(); // Invalid eax
		return -1;
	}
}

unsigned int system_call_exit(unsigned int return_value)
{
	return exit_application(return_value, get_current_task()->task_status_segment.esp0);
}

unsigned int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count)
{
	Shell *shell = get_current_shell();
	unsigned int counter = 0;
	switch(file_descriptor)
	{
	case STDOUT:
	case STDERR:
		for(void const *reader = buffer; reader != buffer + count; reader++)
		{
			Event event;
			event.type = EVENT_TYPE_SHELL_PUT_CHARACTER;
			event.event_union.shell_put_character_event.character = *(char const *)reader;
			event.event_union.shell_put_character_event.shell = shell;
			enqueue(shell->event_queue, &event);
			counter++;
		}
		break;
	default:
		ERROR(); // Invalid file descriptor
		printf_serial("Invalid file descriptor %#010.8x\n", file_descriptor);
		break;
	}
	return counter;
}

