// Bibliography
// http://faculty.nps.edu/cseagle/assembly/sys_call.html
// https://rninche01.tistory.com/entry/Linux-system-call-table-%EC%A0%95%EB%A6%ACx86-x64

#include "common.h"
#include "disk.h"
#include "event.h"
#include "io.h"
#include "math.h"
#include "memory.h"
#include "rtc.h"
#include "shell.h"
#include "string.h"
#include "system_call.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

typedef struct _ApplicationWindow
{
	Window *window;
	struct _ApplicationWindow *previous;
	struct _ApplicationWindow *next;
} ApplicationWindow;

typedef struct _ApplicationWindowDeletionResponseEvent
{
	Window *window;
} ApplicationWindowDeletionResponseEvent;

typedef union _ApplicationEventUnion
{
	ApplicationWindowDeletionResponseEvent window_deletion_response_event;
} ApplicationEventUnion;

typedef struct _ApplicationEvent
{
	ApplicationEventUnion event_union;
	unsigned char type;
	#define APPLICATION_EVENT_TYPE_NOTHING			0x00
	#define APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE	0x01
} ApplicationEvent;

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
	Queue *application_event_queue;
	struct _SystemCallStatus *previous;
	struct _SystemCallStatus *next;
} SystemCallStatus;

typedef struct _ConsoleCommand
{
	unsigned char type;
	#define CONSOLE_COMMAND_CLEAR	0x00
} ConsoleCommand;

typedef struct _CPUCommand
{
	unsigned char type;
	#define CPU_COMMAND_HLT	0x0
} CPUCommand;

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
	#define WINDOW_COMMAND_DEQUEUE_EVENT	0x01
	#define WINDOW_COMMAND_DRAW_LINE	0x02
	#define WINDOW_COMMAND_FILL_BOX		0x03
	#define WINDOW_COMMAND_PRINT		0x04
	#define WINDOW_COMMAND_PROCESS_EVENT	0x05
	#define WINDOW_COMMAND_PUT_DOT		0x06
} WindowCommand;

ApplicationWindow *application_windows = NULL;
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
			delete_queue(system_call_status->application_event_queue);
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
	system_call_status->application_event_queue = create_queue(sizeof(ApplicationEvent), get_current_task());
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
			if(!strcmp(file_descriptor->file_name, file_name) && file_descriptor->file_opener_task == get_current_task()) // The caller application opened the same file.
			{
				// Reuse file_descriptor.
				file_descriptor->flags |= flags;
				return file_descriptor;
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
	SystemCallStatus *system_call_status = get_system_call_status();
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
			else if(!strcmp(file_descriptor->file_name, cpu_file_name)) // Control the CPU.
			{
				CPUCommand const * const command = buffer;
				switch(command->type)
				{
				case CPU_COMMAND_HLT:
					sleep_task(get_current_task());
					break;
				default:
					ERROR(); // Invalid CPU command.
					break;
				}
			}
			else if(!strcmp(file_descriptor->file_name, memory_file_name)) // Control the memory.
			{
				MemoryCommand const * const command = buffer;
				unsigned int free_memory_space_size = get_free_memory_space_size();
				if(file_descriptor->buffer_begin)
				{
					free(file_descriptor->buffer_begin);
					file_descriptor->buffer_begin = NULL;
					file_descriptor->buffer_cursor = NULL;
					file_descriptor->buffer_end = NULL;
				}
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
			else if(!strcmp(file_descriptor->file_name, timer_file_name)) // Control timer.
			{
				TimerCommand const * const command = buffer;
				unsigned int unix_time = get_unix_time();
				if(file_descriptor->buffer_begin)
				{
					free(file_descriptor->buffer_begin);
					file_descriptor->buffer_begin = NULL;
					file_descriptor->buffer_cursor = NULL;
					file_descriptor->buffer_end = NULL;
				}
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
			else if(!strcmp(file_descriptor->file_name, window_file_name)) // Control windows.
			{
				ApplicationEvent *application_event;
				ApplicationEvent new_application_event;
				ApplicationWindow *application_window;
				Event const *event;
				Window *window;
				WindowCommand const * const command = buffer;
				if(file_descriptor->buffer_begin)
				{
					free(file_descriptor->buffer_begin);
					file_descriptor->buffer_begin = NULL;
					file_descriptor->buffer_cursor = NULL;
					file_descriptor->buffer_end = NULL;
				}
				switch(command->type)
				{
				case WINDOW_COMMAND_CREATE:
					window = create_window(command->arguments.create.title + application_memory, background_sheet, command->arguments.create.x, command->arguments.create.y, command->arguments.create.width + 2 * EDGE_WIDTH, command->arguments.create.height + TITLE_SHEET_HEIGHT + 3 * EDGE_WIDTH, get_current_task()->event_queue);
					application_window = malloc(sizeof(*application_window));
					application_window->window = window;
					if(application_windows)
					{
						application_window->previous = application_windows->previous;
						application_window->next = application_windows;
						application_windows->previous->next = application_window;
						application_windows->previous = application_window;
					}
					else
					{
						application_window->previous = application_window;
						application_window->next = application_window;
						application_windows = application_window;
					}
					file_descriptor->buffer_begin = malloc(sizeof(window));
					*(Window **)file_descriptor->buffer_begin = window;
					file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
					file_descriptor->buffer_end = (void *)((size_t)file_descriptor->buffer_begin + sizeof(window));
					break;
				case WINDOW_COMMAND_DEQUEUE_EVENT:
					application_event = dequeue(system_call_status->application_event_queue);
					if(!application_event)
					{
						new_application_event.type = APPLICATION_EVENT_TYPE_NOTHING;
						application_event = &new_application_event;
					}
					file_descriptor->buffer_begin = malloc(sizeof(*application_event));
					*(ApplicationEvent *)file_descriptor->buffer_begin = *application_event;
					file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
					file_descriptor->buffer_end = (void *)((size_t)file_descriptor->buffer_begin + sizeof(*application_event));
					break;
				case WINDOW_COMMAND_DRAW_LINE:
					if(sheet_exists(command->arguments.draw_line.window->client_sheet))
					{
						short left_x = command->arguments.draw_line.x1 < command->arguments.draw_line.x2 ? command->arguments.draw_line.x1 : command->arguments.draw_line.x2;
						short right_x = command->arguments.draw_line.x1 < command->arguments.draw_line.x2 ? command->arguments.draw_line.x2 : command->arguments.draw_line.x1;
						short top_x = command->arguments.draw_line.y1 < command->arguments.draw_line.y2 ? command->arguments.draw_line.x1 : command->arguments.draw_line.x2;
						short top_y = command->arguments.draw_line.y1 < command->arguments.draw_line.y2 ? command->arguments.draw_line.y1 : command->arguments.draw_line.y2;
						short bottom_x = command->arguments.draw_line.y1 < command->arguments.draw_line.y2 ? command->arguments.draw_line.x2 : command->arguments.draw_line.x1;
						short bottom_y = command->arguments.draw_line.y1 < command->arguments.draw_line.y2 ? command->arguments.draw_line.y2 : command->arguments.draw_line.y1;
						if(right_x - left_x < bottom_y - top_y)for(short y = top_y; y <= bottom_y; y++)
						{
							short x = ((bottom_x - top_x) * y + top_x * bottom_y - bottom_x * top_y) / (bottom_y - top_y);
							put_dot_sheet(command->arguments.draw_line.window->client_sheet, x, y, command->arguments.draw_line.color);
						}
						else for(short x = left_x; x <= right_x; x++)
						{
							short y = top_x == bottom_x ? top_y : ((top_y - bottom_y) * x + top_x * bottom_y - bottom_x * top_y) / (top_x - bottom_x);
							put_dot_sheet(command->arguments.draw_line.window->client_sheet, x, y, command->arguments.draw_line.color);
						}
					}
					break;
				case WINDOW_COMMAND_FILL_BOX:
					if(sheet_exists(command->arguments.fill_box.window->client_sheet))fill_box_sheet(command->arguments.fill_box.window->client_sheet, command->arguments.fill_box.x, command->arguments.fill_box.y, command->arguments.fill_box.width, command->arguments.fill_box.height, command->arguments.fill_box.color);
					break;
				case WINDOW_COMMAND_PRINT:
					if(sheet_exists(command->arguments.print.window->client_sheet))print_sheet(command->arguments.print.window->client_sheet, command->arguments.print.x, command->arguments.print.y, command->arguments.print.foreground, command->arguments.print.background, command->arguments.print.string + application_memory);
					break;
				case WINDOW_COMMAND_PROCESS_EVENT:
					event = dequeue(get_current_task()->event_queue);
					if(event)
					{
						switch(event->type)
						{
						case EVENT_TYPE_CLOSE_BUTTON_CLICKED:
						case EVENT_TYPE_SHEET_CLICKED:
						case EVENT_TYPE_SHEET_CREATED:
						case EVENT_TYPE_SHEET_DELETION_REQUEST:
						case EVENT_TYPE_SHEET_DELETION_RESPONSE:
						case EVENT_TYPE_SHEET_FOCUSED:
						case EVENT_TYPE_SHEET_KEYBOARD:
						case EVENT_TYPE_SHEET_MOUSE_DRAG:
						case EVENT_TYPE_SHEET_MOUSE_MOVE:
						case EVENT_TYPE_SHEET_UNFOCUSED:
						case EVENT_TYPE_SHEET_USER_DEFINED:
						case EVENT_TYPE_SHEET_VERTICAL_WHEEL:
						case EVENT_TYPE_WINDOW_DELETION_REQUEST:
						case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
						case EVENT_TYPE_WINDOW_FOCUSED:
						case EVENT_TYPE_WINDOW_UNFOCUSED:
							distribute_event(event);
							break;
						default:
							ERROR();	// Irrelevant event type.
							break;
						}
						switch(event->type)
						{
						case EVENT_TYPE_SHEET_CREATED:
							printf_shell(shell, "sheet created.\n");
							break;
						case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
							application_window = application_windows;
							do
							{
								if(application_window->window == event->event_union.window_deletion_response_event.window)
								{
									if(application_window == application_windows)application_windows = application_window->next;
									if(application_window == application_windows)application_windows = NULL;
									application_window->previous->next = application_window->next;
									application_window->next->previous = application_window->previous;
									free(application_window);
									break;
								}
								application_window = application_window->next;
							} while(application_window != application_windows);
							new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE;
							new_application_event.event_union.window_deletion_response_event.window = event->event_union.window_deletion_response_event.window;
							enqueue(system_call_status->application_event_queue, &new_application_event);
							break;
						}
					}
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

