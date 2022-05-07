// Bibliography
// http://faculty.nps.edu/cseagle/assembly/sys_call.html
// https://rninche01.tistory.com/entry/Linux-system-call-table-%EC%A0%95%EB%A6%ACx86-x64

#include "common.h"
#include "event.h"
#include "io.h"
#include "shell.h"
#include "string.h"
#include "system_call.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

int system_call_exit(int return_value);
unsigned int system_call_open(char const *file_name, unsigned int flags);
#define SYSTEM_CALL_OPEN_FLAG_READ_ONLY 0x01
int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

int system_call(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	UNUSED_ARGUMENT(esi);
	UNUSED_ARGUMENT(edi);
	UNUSED_ARGUMENT(ebp);
	#define SYSTEM_CALL_EXIT	0x00000001
	#define SYSTEM_CALL_WRITE	0x00000004
	#define SYSTEM_CALL_OPEN	0x00000005
	switch(eax)
	{
	case SYSTEM_CALL_EXIT:
		return system_call_exit(ebx);
	case SYSTEM_CALL_WRITE:
		return system_call_write((unsigned int)ebx, (void const *)(ecx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (size_t)edx);
	case SYSTEM_CALL_OPEN:
		return (int)system_call_open((char const *)(ebx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (unsigned int)ecx);
	default:
		ERROR(); // Invalid eax
		return -1;
	}
}

int system_call_exit(int return_value)
{
	return exit_application(return_value, get_current_task()->task_status_segment.esp0);
}

unsigned int system_call_open(char const *file_name, unsigned int flags)
{
	if(!strcmp(file_name, "") && flags == SYSTEM_CALL_OPEN_FLAG_READ_ONLY)system_call_write(STDOUT, "Open root directory!\n", 21);
	return 0;
}

int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count)
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

