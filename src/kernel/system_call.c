// Bibliography
// http://faculty.nps.edu/cseagle/assembly/sys_call.html
// https://rninche01.tistory.com/entry/Linux-system-call-table-%EC%A0%95%EB%A6%ACx86-x64

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
	UNUSED_ARGUMENT(esi);
	UNUSED_ARGUMENT(edi);
	UNUSED_ARGUMENT(ebp);
	#define SYSTEM_CALL_EXIT	0x00000001
	#define SYSTEM_CALL_WRITE	0x00000004
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
		break;
	}
	return counter;
}

