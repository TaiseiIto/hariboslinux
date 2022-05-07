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

typedef struct _FileStatus
{
	char *file_name;
	Task *file_opener_task;
	unsigned int file_descriptor;
	unsigned int flags;
	#define SYSTEM_CALL_OPEN_FLAG_READ	0x00000001
	#define SYSTEM_CALL_OPEN_FLAG_WRITE	0x00000002
	struct _FileStatus *previous;
	struct _FileStatus *next;
} FileStatus;

typedef struct _SystemCallStatus
{
	Task *application_task;
	struct _SystemCallStatus *previous;
	struct _SystemCallStatus *next;
} SystemCallStatus;

FileStatus *file_statuses = NULL;
SystemCallStatus *system_call_statuses = NULL;

void delete_system_call_status(void);
SystemCallStatus *get_system_call_status(void);
int system_call_exit(int return_value);
unsigned int system_call_open(char const *file_name, unsigned int flags);
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
	FileStatus *file_status;
	if(file_statuses)
	{
		unsigned int max_file_descriptor = STDERR; // The max file descriptor opened by the caller application.
		file_status = file_statuses;
		do
		{
			if(!strcmp(file_status->file_name, file_name)) // Someone opened the same file.
			{
				if(file_status->file_opener_task == get_current_task()) // The caller application opened the same file.
				{
					// Reuse file_status.
					file_status->flags |= flags;
					return file_status->file_descriptor;
				}
				else // Another application opened the same file.
				{
					if(file_status->flags & SYSTEM_CALL_OPEN_FLAG_WRITE || flags & SYSTEM_CALL_OPEN_FLAG_WRITE)return 0; // Can't open the file with writing mode.
				}
			}
			if(file_status->file_opener_task == get_current_task() && max_file_descriptor < file_status->file_descriptor)max_file_descriptor = file_status->file_descriptor;
			file_status = file_status->next;
		} while(file_status != file_statuses);
		file_status = malloc(sizeof(*file_status));
		file_status->file_name = malloc(strlen(file_name) + 1);
		strcpy(file_status->file_name, file_name);
		file_status->file_opener_task = get_current_task();
		file_status->file_descriptor = max_file_descriptor + 1;
		file_status->flags = flags;
		file_status->previous = file_statuses->previous;
		file_status->next = file_statuses;
		file_statuses->previous->next = file_status;
		file_statuses->previous = file_status;
	}
	else
	{
		file_status = malloc(sizeof(*file_status));
		file_status->file_name = malloc(strlen(file_name) + 1);
		strcpy(file_status->file_name, file_name);
		file_status->file_opener_task = get_current_task();
		file_status->file_descriptor = STDERR + 1;
		file_status->flags = flags;
		file_status->previous = file_status;
		file_status->next = file_status;
		file_statuses = file_status;
	}
	return file_status->file_descriptor;
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

