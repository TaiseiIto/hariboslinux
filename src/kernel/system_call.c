// Bibliography
// http://faculty.nps.edu/cseagle/assembly/sys_call.html
// https://rninche01.tistory.com/entry/Linux-system-call-table-%EC%A0%95%EB%A6%ACx86-x64

#include "common.h"
#include "disk.h"
#include "event.h"
#include "io.h"
#include "memory.h"
#include "rtc.h"
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
	void *buffer_begin;
	void *buffer_cursor;
	void *buffer_end;
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

typedef struct _ConsoleCommand
{
	unsigned char type;
	#define CONSOLE_COMMAND_CLEAR	0x00
} ConsoleCommand;

typedef struct _MemoryCommand
{
	unsigned char type;
	#define MEMORY_COMMAND_FREE	0x00
} MemoryCommand;

typedef struct _TimerCommand
{
	unsigned char type;
	#define TIMER_COMMAND_GET	0x00
} TimerCommand;

typedef struct _WindowCommandCreateArguments
{
	char const *title;
	short x;
	short y;
	unsigned short width;
	unsigned short height;
} WindowCommandCreateArguments;

typedef struct _WindowCommandDrawLine
{
	Window *window;
	short x1;
	short y1;
	short x2;
	short y2;
	Color color;
} WindowCommandDrawLine;

typedef struct _WindowCommandFillBox
{
	Window *window;
	short x;
	short y;
	unsigned short width;
	unsigned short height;
	Color color;
} WindowCommandFillBox;

typedef struct _WindowCommandPrint
{
	Window *window;
	short x;
	short y;
	Color foreground;
	Color background;
	char const *string;
} WindowCommandPrint;

typedef struct _WindowCommandPutDot
{
	Window *window;
	unsigned short x;
	unsigned short y;
	Color color;
} WindowCommandPutDot;

typedef union _WindowCommandArguments
{
	WindowCommandCreateArguments create;
	WindowCommandDrawLine draw_line;
	WindowCommandFillBox fill_box;
	WindowCommandPrint print;
	WindowCommandPutDot put_dot;
} WindowCommandArguments;

typedef struct _WindowCommand
{
	WindowCommandArguments arguments;
	unsigned char type;
	#define WINDOW_COMMAND_CREATE		0x00
	#define WINDOW_COMMAND_DRAW_LINE	0x01
	#define WINDOW_COMMAND_FILL_BOX		0x02
	#define WINDOW_COMMAND_PRINT		0x03
	#define WINDOW_COMMAND_PUT_DOT		0x04
} WindowCommand;

FileDescriptor *file_descriptors = NULL;
SystemCallStatus *system_call_statuses = NULL;

void delete_file_descriptors(void);
void delete_system_call_status(void);
SystemCallStatus *get_system_call_status(void);
int system_call_close(FileDescriptor *file_descriptor);
int system_call_exit(int return_value);
FileDescriptor *system_call_open(char const *file_name, unsigned int flags);
size_t system_call_read(FileDescriptor *file_descriptor, void *buffer, size_t count);
int system_call_write(FileDescriptor *file_descriptor, void const *buffer, size_t count);

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
			if(file_descriptor->buffer_begin)free(file_descriptor->buffer_begin);
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
	#define SYSTEM_CALL_READ	0x00000003
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
	case SYSTEM_CALL_READ:
		return_value = system_call_read((FileDescriptor *)ebx, (void *)(ecx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (size_t)edx);
		break;
	case SYSTEM_CALL_WRITE:
		return_value = system_call_write((FileDescriptor *)ebx, (void const *)(ecx + (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory), (size_t)edx);
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
			if(file_descriptor->buffer_begin)free(file_descriptor->buffer_begin);
			free(file_descriptor);
			return 0;
		}
		file_descriptor_finder = file_descriptor_finder->next;
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
		file_descriptor->previous = file_descriptors->previous;
		file_descriptor->next = file_descriptors;
		file_descriptors->previous->next = file_descriptor;
		file_descriptors->previous = file_descriptor;
	}
	else
	{
		file_descriptor = malloc(sizeof(*file_descriptor));
		file_descriptor->previous = file_descriptor;
		file_descriptor->next = file_descriptor;
		file_descriptors = file_descriptor;
	}
	file_descriptor->file_name = malloc(strlen(file_name) + 1);
	strcpy(file_descriptor->file_name, file_name);
	file_descriptor->file_opener_task = get_current_task();
	file_descriptor->buffer_begin = load_file(file_descriptor->file_name);
	file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
	file_descriptor->buffer_end = (void *)((unsigned int)file_descriptor->buffer_begin + get_file_size(file_descriptor->file_name));
	file_descriptor->flags = flags;
	return file_descriptor;
}

size_t system_call_read(FileDescriptor *file_descriptor, void *buffer, size_t count)
{
	if(file_descriptor->flags & SYSTEM_CALL_OPEN_FLAG_READ)
	{
		FileDescriptor *file_descriptor_finder = file_descriptors;
		if(file_descriptor_finder)do
		{
			if(file_descriptor_finder == file_descriptor)
			{
				size_t read_size = count < (size_t)file_descriptor->buffer_end - (size_t)file_descriptor->buffer_cursor ? count : (size_t)file_descriptor->buffer_end - (size_t)file_descriptor->buffer_cursor;
				memcpy(buffer, file_descriptor->buffer_cursor, read_size);
				file_descriptor->buffer_cursor += read_size;
				return read_size;
			}
			file_descriptor_finder = file_descriptor_finder->next;
		} while(file_descriptors && file_descriptor_finder != file_descriptors);
		ERROR(); // There is no such a file_descriptor.
	}
	return 0;
}

int system_call_write(FileDescriptor *file_descriptor, void const *buffer, size_t count)
{
	unsigned int counter = 0;
	unsigned int application_memory = (unsigned int)((CommandTaskAdditional *)get_current_task()->additionals)->application_memory;
	if(file_descriptor->flags & SYSTEM_CALL_OPEN_FLAG_WRITE)
	{
		Shell *shell = get_current_shell();
		switch((unsigned int)file_descriptor)
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
			if(!strcmp(file_descriptor->file_name, console_file_name)) // Control the console.
			{
				Console *console;
				TextBox *text_box;
				ConsoleCommand const * const command = buffer;
				switch(command->type)
				{
				case CONSOLE_COMMAND_CLEAR:
					switch(shell->type)
					{
					case SHELL_TYPE_CONSOLE:
						console = shell->console;
						text_box = console->text_box;
						text_box_delete_chars(text_box, text_box->first_position, text_box->string->length);
						break;
					case SHELL_TYPE_SERIAL:
						break;
					default:
						ERROR(); // Invalid shell type.
					}
					break;
				default:
					ERROR(); // Invalid console command.
					break;
				}
			}
			if(!strcmp(file_descriptor->file_name, memory_file_name)) // Control the memory.
			{
				MemoryCommand const * const command = buffer;
				unsigned int free_memory_space_size = get_free_memory_space_size();
				if(file_descriptor->buffer_begin)free(file_descriptor->buffer_begin);
				switch(command->type)
				{
				case MEMORY_COMMAND_FREE:
					file_descriptor->buffer_begin = malloc(sizeof(free_memory_space_size));
					*(unsigned int *)file_descriptor->buffer_begin = free_memory_space_size;
					file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
					file_descriptor->buffer_end = (void *)((size_t)file_descriptor->buffer_begin + sizeof(free_memory_space_size));
					break;
				default:
					ERROR(); // Invalid console command.
					break;
				}
			}
			if(!strcmp(file_descriptor->file_name, timer_file_name)) // Control timer.
			{
				TimerCommand const * const command = buffer;
				unsigned int unix_time = get_unix_time();
				if(file_descriptor->buffer_begin)free(file_descriptor->buffer_begin);
				switch(command->type)
				{
				case TIMER_COMMAND_GET:
					file_descriptor->buffer_begin = malloc(sizeof(unix_time));
					*(unsigned int *)file_descriptor->buffer_begin = unix_time;
					file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
					file_descriptor->buffer_end = (void *)((size_t)file_descriptor->buffer_begin + sizeof(unix_time));
					break;
				default:
					ERROR(); // Invalid console command.
					break;
				}
			}
			if(!strcmp(file_descriptor->file_name, window_file_name)) // Control windows.
			{
				Window *window;
				WindowCommand const * const command = buffer;
				if(file_descriptor->buffer_begin)free(file_descriptor->buffer_begin);
				switch(command->type)
				{
				case WINDOW_COMMAND_CREATE:
					window = create_window(command->arguments.create.title + application_memory, background_sheet, command->arguments.create.x, command->arguments.create.y, command->arguments.create.width + 2 * EDGE_WIDTH, command->arguments.create.height + TITLE_SHEET_HEIGHT + 3 * EDGE_WIDTH, main_task.event_queue);
					file_descriptor->buffer_begin = malloc(sizeof(window));
					*(Window **)file_descriptor->buffer_begin = window;
					file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
					file_descriptor->buffer_end = (void *)((size_t)file_descriptor->buffer_begin + sizeof(window));
					break;
				case WINDOW_COMMAND_DRAW_LINE:
					if(sheet_exists(command->arguments.draw_line.window->client_sheet))
					{
						printf_shell(shell, "Draw line\n");
						printf_shell(shell, "window = %p\n", command->arguments.draw_line.window);
						printf_shell(shell, "x1 = %u\n", command->arguments.draw_line.x1);
						printf_shell(shell, "y1 = %u\n", command->arguments.draw_line.y1);
						printf_shell(shell, "x2 = %u\n", command->arguments.draw_line.x2);
						printf_shell(shell, "y2 = %u\n", command->arguments.draw_line.y2);
						printf_shell(shell, "color.red = %#06.4x\n", command->arguments.draw_line.color.red);
						printf_shell(shell, "color.green = %#06.4x\n", command->arguments.draw_line.color.green);
						printf_shell(shell, "color.blue = %#06.4x\n", command->arguments.draw_line.color.blue);
						printf_shell(shell, "color.alpha = %#06.4x\n", command->arguments.draw_line.color.alpha);
					}
					break;
				case WINDOW_COMMAND_FILL_BOX:
					if(sheet_exists(command->arguments.fill_box.window->client_sheet))fill_box_sheet(command->arguments.fill_box.window->client_sheet, command->arguments.fill_box.x, command->arguments.fill_box.y, command->arguments.fill_box.width, command->arguments.fill_box.height, command->arguments.fill_box.color);
					break;
				case WINDOW_COMMAND_PRINT:
					if(sheet_exists(command->arguments.print.window->client_sheet))print_sheet(command->arguments.print.window->client_sheet, command->arguments.print.x, command->arguments.print.y, command->arguments.print.foreground, command->arguments.print.background, command->arguments.print.string + application_memory);
					break;
				case WINDOW_COMMAND_PUT_DOT:
					if(sheet_exists(command->arguments.put_dot.window->client_sheet))put_dot_sheet(command->arguments.put_dot.window->client_sheet, command->arguments.put_dot.x, command->arguments.put_dot.y, command->arguments.put_dot.color);
					break;
				default:
					ERROR(); // Invalid console command.
					break;
				}
			}
			break;
		}
	}
	return counter;
}

