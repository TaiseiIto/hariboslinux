// Bibliography
// http://faculty.nps.edu/cseagle/assembly/sys_call.html
// https://rninche01.tistory.com/entry/Linux-system-call-table-%EC%A0%95%EB%A6%ACx86-x64

#include "common.h"
#include "event.h"
#include "io.h"
#include "memory.h"
#include "shell.h"
#include "string.h"
#include "system_call.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

typedef struct _SystemCallStatus
{
	Task *application_task;
	struct _SystemCallStatus *previous;
	struct _SystemCallStatus *next;
} SystemCallStatus;

SystemCallStatus *system_call_statuses = NULL;

void delete_system_call_status(void);
SystemCallStatus *get_system_call_status(void);
int system_call_exit(int return_value);
unsigned int system_call_open(char const *file_name, unsigned int flags);
#define SYSTEM_CALL_OPEN_FLAG_READ_ONLY 0x01
int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

void delete_system_call_status(void)
{
	if(system_call_statuses)
	{
		SystemCallStatus *system_call_status = system_call_statuses;
		do
		{
			if(system_call_status->application_task == get_current_task())
			{
				if(system_call_statuses == system_call_status)system_call_statuses = system_call_status->next;
				if(system_call_statuses == system_call_status)system_call_statuses = NULL;
				system_call_status->previous->next = system_call_status->next;
				system_call_status->next->previous = system_call_status->previous;
				free(system_call_status);
				break;
			}
			system_call_status = system_call_status->next;
		} while(system_call_status != system_call_statuses);
	}
}

SystemCallStatus *get_system_call_status(void)
{
	SystemCallStatus *system_call_status = system_call_statuses;
	if(system_call_status)do
	{
		if(system_call_status->application_task == get_current_task())return system_call_status;
		system_call_status = system_call_status->next;
	} while(system_call_status != system_call_statuses);
	system_call_status = malloc(sizeof(*system_call_status));
	system_call_status->application_task = get_current_task();
	if(system_call_statuses)
	{
		system_call_status->previous = system_call_statuses->previous;
		system_call_status->next = system_call_statuses;
		system_call_statuses->previous->next = system_call_status;
		system_call_statuses->previous = system_call_status;
	}
	else
	{
		system_call_status->previous = system_call_status;
		system_call_status->next = system_call_status;
		system_call_statuses = system_call_status;
	}
	return system_call_status;
}

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
	// Delete system call status of the application
	delete_system_call_status();
	// Exit the application
	return exit_application(return_value, get_current_task()->task_status_segment.esp0);
}

unsigned int system_call_open(char const *file_name, unsigned int flags)
{
	Shell *shell = get_current_shell();
	SystemCallStatus *system_call_status = get_system_call_status();
	printf_shell(shell, "system_call_status = %p\n", system_call_status);
	if(!strcmp(file_name, "") && flags == SYSTEM_CALL_OPEN_FLAG_READ_ONLY)printf_shell(shell, "Open root directory!\n");
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

