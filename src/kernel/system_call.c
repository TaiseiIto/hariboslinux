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

typedef struct _FileDescriptor
{
	char *file_name;
	Task *file_opener_task;
	unsigned int flags;
	#define SYSTEM_CALL_OPEN_FLAG_READ	0x00000001
	#define SYSTEM_CALL_OPEN_FLAG_WRITE	0x00000002
	struct _FileDescriptor *previous;
	struct _FileDescriptor *next;
} FileDescriptor;

typedef struct _SystemCallStatus
{
	Task *application_task;
	struct _SystemCallStatus *previous;
	struct _SystemCallStatus *next;
} SystemCallStatus;

FileDescriptor *file_descriptors = NULL;
SystemCallStatus *system_call_statuses = NULL;

void delete_file_descriptors(void);
void delete_system_call_status(void);
SystemCallStatus *get_system_call_status(void);
int system_call_close(FileDescriptor *file_descriptor);
int system_call_exit(int return_value);
FileDescriptor *system_call_open(char const *file_name, unsigned int flags);
int system_call_write(unsigned int file_descriptor, void const *buffer, size_t count);

void delete_file_descriptors(void)
{
	FileDescriptor *file_descriptor = file_descriptors;
	if(file_descriptor)do
	{
		FileDescriptor *next_file_descriptor = file_descriptor->next;
		if(file_descriptor->file_opener_task == get_current_task())
		{
			if(file_descriptors == file_descriptor)file_descriptors = file_descriptor->next;
			if(file_descriptors == file_descriptor)file_descriptors = NULL;
			file_descriptor->previous->next = file_descriptor->next;
			file_descriptor->next->previous = file_descriptor->previous;
			free(file_descriptor->file_name);
			free(file_descriptor);
		}
		file_descriptor = next_file_descriptor;
	} while(file_descriptors && file_descriptor != file_descriptors);
}

void delete_system_call_status(void)
{
	SystemCallStatus *system_call_status = system_call_statuses;
	if(system_call_status)do
	{
		SystemCallStatus *next_system_call_status = system_call_status->next;
		if(system_call_status->application_task == get_current_task())
		{
			if(system_call_statuses == system_call_status)system_call_statuses = system_call_status->next;
			if(system_call_statuses == system_call_status)system_call_statuses = NULL;
			system_call_status->previous->next = system_call_status->next;
			system_call_status->next->previous = system_call_status->previous;
			free(system_call_status);
			break;
		}
		system_call_status = next_system_call_status;
	} while(system_call_statuses && system_call_status != system_call_statuses);
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
	int return_value;
	prohibit_switch_task();
	UNUSED_ARGUMENT(esi);
	UNUSED_ARGUMENT(edi);
	UNUSED_ARGUMENT(ebp);
	#define SYSTEM_CALL_EXIT	0x00000001
	#define SYSTEM_CALL_WRITE	0x00000004
	#define SYSTEM_CALL_OPEN	0x00000005
	#define SYSTEM_CALL_CLOSE	0x00000006
	switch(eax)
	{
	case SYSTEM_CALL_CLOSE:
		return_value = system_call_close((FileDescriptor *)ebx);
		break;
	case SYSTEM_CALL_EXIT:
		allow_switch_task();
		return_value = system_call_exit(ebx);
		break;
	case SYSTEM_CALL_OPEN:
		return_value = (int)system_call_open((char const *)(ebx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (unsigned int)ecx);
		break;
	case SYSTEM_CALL_WRITE:
		return_value = system_call_write((unsigned int)ebx, (void const *)(ecx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (size_t)edx);
		break;
	default:
		ERROR(); // Invalid eax
		return_value = -1;
		break;
	}
	allow_switch_task();
	return return_value;
}

int system_call_close(FileDescriptor *file_descriptor)
{
	FileDescriptor *file_descriptor_finder = file_descriptors;
	if(file_descriptor_finder)do
	{
		if(file_descriptor_finder == file_descriptor)
		{
			if(file_descriptors == file_descriptor)file_descriptors = file_descriptor->next;
			if(file_descriptors == file_descriptor)file_descriptors = NULL;
			file_descriptor->previous->next = file_descriptor->next;
			file_descriptor->next->previous = file_descriptor->previous;
			free(file_descriptor->file_name);
			free(file_descriptor);
		}
	} while(file_descriptors && file_descriptor_finder != file_descriptors);
	ERROR(); // There is no such a file_descriptor.
	return -1;
}

int system_call_exit(int return_value)
{
	// Delete file descriptors of the application.
	delete_file_descriptors();
	// Delete system call status of the application.
	delete_system_call_status();
	// Exit the application.
	return exit_application(return_value, get_current_task()->task_status_segment.esp0);
}

FileDescriptor *system_call_open(char const *file_name, unsigned int flags)
{
	FileDescriptor *file_descriptor;
	if(file_descriptors)
	{
		file_descriptor = file_descriptors;
		do
		{
			if(!strcmp(file_descriptor->file_name, file_name)) // Someone opened the same file.
			{
				if(file_descriptor->file_opener_task == get_current_task()) // The caller application opened the same file.
				{
					// Reuse file_descriptor.
					file_descriptor->flags |= flags;
					return file_descriptor;
				}
				else // Another application opened the same file.
				{
					if(file_descriptor->flags & SYSTEM_CALL_OPEN_FLAG_WRITE || flags & SYSTEM_CALL_OPEN_FLAG_WRITE)return 0; // Can't open the file with writing mode.
				}
			}
			file_descriptor = file_descriptor->next;
		} while(file_descriptor != file_descriptors);
		file_descriptor = malloc(sizeof(*file_descriptor));
		file_descriptor->file_name = malloc(strlen(file_name) + 1);
		strcpy(file_descriptor->file_name, file_name);
		file_descriptor->file_opener_task = get_current_task();
		file_descriptor->flags = flags;
		file_descriptor->previous = file_descriptors->previous;
		file_descriptor->next = file_descriptors;
		file_descriptors->previous->next = file_descriptor;
		file_descriptors->previous = file_descriptor;
	}
	else
	{
		file_descriptor = malloc(sizeof(*file_descriptor));
		file_descriptor->file_name = malloc(strlen(file_name) + 1);
		strcpy(file_descriptor->file_name, file_name);
		file_descriptor->file_opener_task = get_current_task();
		file_descriptor->flags = flags;
		file_descriptor->previous = file_descriptor;
		file_descriptor->next = file_descriptor;
		file_descriptors = file_descriptor;
	}
	return file_descriptor;
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

