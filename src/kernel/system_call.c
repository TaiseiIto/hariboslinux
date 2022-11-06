// Bibliography
// http://faculty.nps.edu/cseagle/assembly/sys_call.html
// https://rninche01.tistory.com/entry/Linux-system-call-table-%EC%A0%95%EB%A6%ACx86-x64

#include "acpi.h"
#include "common.h"
#include "disk.h"
#include "event.h"
#include "fpu.h"
#include "io.h"
#include "math.h"
#include "memory.h"
#include "rtc.h"
#include "shell.h"
#include "stdlib.h"
#include "string.h"
#include "system_call.h"

#define STDIN	0x00000000
#define STDOUT	0x00000001
#define STDERR	0x00000002

typedef struct _ApplicationTimer
{
	Timer *timer;
	Task *owner_task;
	struct _ApplicationTimer *previous;
	struct _ApplicationTimer *next;
} ApplicationTimer;

typedef struct _ApplicationWindow
{
	Window *window;
	Task *owner_task;
	struct _ApplicationWindow *previous;
	struct _ApplicationWindow *next;
} ApplicationWindow;

typedef struct _ApplicationTimerEvent
{
	Timer *timer;
} ApplicationTimerEvent;

typedef struct _ApplicationWindowClickedEvent
{
	Window *window;
	unsigned short x, y;
	unsigned char flags;
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_PUSHED		0x01
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_RELEASED		0x02
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_LEFT_BUTTON	0x04
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_MIDDLE_BUTTON	0x08
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_RIGHT_BUTTON	0x10
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_4TH_BUTTON	0x20
	#define APPLICATION_WINDOW_CLICKED_EVENT_FLAG_5TH_BUTTON	0x40
} ApplicationWindowClickedEvent;

typedef struct _ApplicationWindowCreatedEvent
{
	Window *window;
} ApplicationWindowCreatedEvent;

typedef struct _ApplicationWindowDeletionResponseEvent
{
	Window *window;
} ApplicationWindowDeletionResponseEvent;

typedef struct _ApplicationWindowDragEvent
{
	Window *window;
	short x_movement, y_movement;
} ApplicationWindowDragEvent;

typedef struct _ApplicationWindowKeyboardEvent
{
	Window *window;
	KeyboardEvent keyboard_event;
} ApplicationWindowKeyboardEvent;

typedef struct _ApplicationWindowMoveEvent
{
	Window *window;
	short x_movement, y_movement;
} ApplicationWindowMoveEvent;

typedef struct _ApplicationWindowVerticalWheelEvelt
{
	Window *window;
	short x, y;
	char rotation;
} ApplicationWindowVerticalWheelEvent;

typedef union _ApplicationEventUnion
{
	ApplicationTimerEvent timer_event;
	ApplicationWindowClickedEvent window_clicked_event;
	ApplicationWindowCreatedEvent window_created_event;
	ApplicationWindowDeletionResponseEvent window_deletion_response_event;
	ApplicationWindowDragEvent window_drag_event;
	ApplicationWindowKeyboardEvent window_keyboard_event;
	ApplicationWindowMoveEvent window_move_event;
	ApplicationWindowVerticalWheelEvent window_vertical_wheel_event;
} ApplicationEventUnion;

typedef struct _ApplicationEvent
{
	ApplicationEventUnion event_union;
	unsigned char type;
	#define APPLICATION_EVENT_TYPE_NOTHING			0x00
	#define APPLICATION_EVENT_TYPE_TIMER			0x01
	#define APPLICATION_EVENT_TYPE_WINDOW_CLICKED		0x02
	#define APPLICATION_EVENT_TYPE_WINDOW_CREATED		0x03
	#define APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE	0x04
	#define APPLICATION_EVENT_TYPE_WINDOW_DRAG		0x05
	#define APPLICATION_EVENT_TYPE_WINDOW_KEYBOARD		0x06
	#define APPLICATION_EVENT_TYPE_WINDOW_MOVE		0x07
	#define APPLICATION_EVENT_TYPE_WINDOW_VERTICAL_WHEEL	0x08
} ApplicationEvent;

typedef struct _FileDescriptor
{
	char *file_name;
	Task *owner_task;
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
	#define CONSOLE_COMMAND_EXIT	0x01
} ConsoleCommand;

typedef struct _CPUCommand
{
	unsigned char type;
	#define CPU_COMMAND_HLT		0x0
	#define CPU_COMMAND_SHUTDOWN	0x1
} CPUCommand;

typedef struct _MemoryCommand
{
	unsigned char type;
	#define MEMORY_COMMAND_FREE	0x00
} MemoryCommand;

typedef struct _TimerCommandCreate
{
	unsigned long long estimated_count;
	unsigned long long interval_count;
} TimerCommandCreate;

typedef union _TimerCommandArguments
{
	TimerCommandCreate create;
} TimerCommandArguments;

typedef struct _TimerCommand
{
	TimerCommandArguments arguments;
	unsigned char type;
	#define TIMER_COMMAND_CREATE	0x00
	#define TIMER_COMMAND_GET	0x01
} TimerCommand;

typedef struct _WindowCommandCreate
{
	char const *title;
	short x;
	short y;
	unsigned short width;
	unsigned short height;
} WindowCommandCreate;

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
	WindowCommandCreate create;
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

ApplicationTimer *application_timers = NULL;
ApplicationWindow *application_windows = NULL;
FileDescriptor *file_descriptors = NULL;
SystemCallStatus *system_call_statuses = NULL;

void *application_timer_procedure(ApplicationTimer const *application_timer);
void delete_file_descriptors(void);
void delete_system_call_status(void);
void delete_timers(void);
void delete_windows(void);
SystemCallStatus *get_system_call_status(void);
ApplicationTimer *get_application_timer_from_timer(Timer const *timer);
ApplicationWindow *get_application_window_from_window(Window const *window);
int system_call_close(FileDescriptor *file_descriptor);
int system_call_exit(int return_value);
FileDescriptor *system_call_open(char const *file_name, unsigned int flags);
size_t system_call_read(FileDescriptor *file_descriptor, void *buffer, size_t count);
int system_call_write(FileDescriptor *file_descriptor, void const *buffer, size_t count);

void *application_timer_procedure(ApplicationTimer const *application_timer)
{
	ApplicationEvent timer_event;
	timer_event.type = APPLICATION_EVENT_TYPE_TIMER;
	timer_event.event_union.timer_event.timer = application_timer->timer;
	enqueue(get_system_call_status()->application_event_queue, &timer_event);
	return NULL;
}

void delete_file_descriptors(void)
{
	FileDescriptor *file_descriptor = file_descriptors;
	if(file_descriptor)do
	{
		FileDescriptor *next_file_descriptor = file_descriptor->next;
		if(file_descriptor->owner_task == get_current_task())
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

void delete_timers(void)
{
	ApplicationTimer *application_timer = application_timers;
	if(application_timer)do
	{
		ApplicationTimer *next_application_timer = application_timer->next;
		if(application_timer->owner_task == get_current_task())
		{
			if(application_timers == application_timer)application_timers = application_timer->next;
			if(application_timers == application_timer)application_timers = NULL;
			application_timer->previous->next = application_timer->next;
			application_timer->next->previous = application_timer->previous;
			delete_timer(application_timer->timer);
			free(application_timer);
		}
		application_timer = next_application_timer;
	} while(application_timers && application_timer != application_timers);
}

void delete_windows(void)
{
	ApplicationWindow *application_window = application_windows;
	if(application_window)do
	{
		ApplicationWindow *next_application_window = application_window->next;
		if(application_window->owner_task == get_current_task())
		{
			Event new_event;
			if(application_windows == application_window)application_windows = application_window->next;
			if(application_windows == application_window)application_windows = NULL;
			application_window->previous->next = application_window->next;
			application_window->next->previous = application_window->previous;
			change_window_event_queue(application_window->window, main_task.event_queue);
			new_event.type = EVENT_TYPE_WINDOW_DELETION_REQUEST;
			new_event.event_union.window_deletion_request_event.window = application_window->window;
			enqueue(main_task.event_queue, &new_event);
			free(application_window);
		}
		application_window = next_application_window;
	} while(application_windows && application_window != application_windows);
}

SystemCallStatus *get_system_call_status(void)
{
	SystemCallStatus *system_call_status = system_call_statuses;
	Task *task = get_current_task();
	if(system_call_status)do
	{
		if(system_call_status->application_task == task)return system_call_status;
		system_call_status = system_call_status->next;
	} while(system_call_status != system_call_statuses);
	system_call_status = malloc(sizeof(*system_call_status));
	system_call_status->application_task = task;
	system_call_status->application_event_queue = create_queue(sizeof(ApplicationEvent), task);
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

ApplicationTimer *get_application_timer_from_timer(Timer const *timer)
{
	ApplicationTimer *application_timer = application_timers;
	if(!application_timer)return NULL;
	do
	{
		if(application_timer->timer == timer)return application_timer;
		application_timer = application_timer->next;
	} while(application_timer != application_timers);
	return NULL;
}

ApplicationWindow *get_application_window_from_window(Window const *window)
{
	ApplicationWindow *application_window = application_windows;
	if(!application_window)return NULL;
	do
	{
		if(application_window->window == window)return application_window;
		application_window = application_window->next;
	} while(application_window != application_windows);
	return NULL;
}

int system_call(int eax, int ebx, int ecx, int edx, int esi, int edi, int ebp)
{
	int return_value;
	Task *task = get_current_task();
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
		return_value = (int)system_call_open((char const *)(ebx + (unsigned int)((CommandTaskAdditional *)task->additionals)->application_memory), (unsigned int)ecx);
		break;
	case SYSTEM_CALL_READ:
		return_value = system_call_read((FileDescriptor *)ebx, (void *)(ecx + (unsigned int)((CommandTaskAdditional *)task->additionals)->application_memory), (size_t)edx);
		break;
	case SYSTEM_CALL_WRITE:
		return_value = system_call_write((FileDescriptor *)ebx, (void const *)(ecx + (unsigned int)((CommandTaskAdditional *)task->additionals)->application_memory), (size_t)edx);
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
	release_fpu();
	delete_file_descriptors();
	delete_system_call_status();
	delete_timers();
	delete_windows();
	// Exit the application.
	return exit_application(return_value, get_current_task()->task_status_segment.esp0);
}

FileDescriptor *system_call_open(char const *file_name, unsigned int flags)
{
	FileDescriptor *file_descriptor;
	Task *task = get_current_task();
	if(file_descriptors)
	{
		file_descriptor = file_descriptors;
		do
		{
			if(!strcmp(file_descriptor->file_name, file_name) && file_descriptor->owner_task == task) // The caller application opened the same file.
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
	file_descriptor->owner_task = task;
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
	Task *task = get_current_task();
	unsigned int counter = 0;
	unsigned int application_memory = (unsigned int)((CommandTaskAdditional *)task->additionals)->application_memory;
	SystemCallStatus *system_call_status = get_system_call_status();
	if(file_descriptor->flags & SYSTEM_CALL_OPEN_FLAG_WRITE)
	{
		Shell *shell = get_current_shell();
		switch((unsigned int)file_descriptor)
		{
		case STDOUT:
		case STDERR:
			if(shell)for(void const *reader = buffer; reader != buffer + count; reader++)
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
				if(shell)switch(command->type)
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
				case CONSOLE_COMMAND_EXIT:
					switch(shell->type)
					{
					case SHELL_TYPE_CONSOLE:
						shell->flags |= SHELL_FLAG_EXIT_REQUEST;
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
					MemoryRegionDescriptor acpi_memory_region_descriptor;
					ACPITableHeader const *dsdt_header;
					ACPITableHeader const *rsdt_header;
					AMLSubstring dsdt_aml;
					AMLSymbol *dsdt_aml_syntax_tree;
					FADT const *fadt;
				case CPU_COMMAND_HLT:
					if(!task->event_queue->read_head)sleep_task(task);
					break;
				case CPU_COMMAND_SHUTDOWN:
					acpi_memory_region_descriptor = get_acpi_memory_region_descriptor();
					printf_serial("acpi_memory_region_descriptor.base = %#018.16llx\n", acpi_memory_region_descriptor.base);
					printf_serial("acpi_memory_region_descriptor.length = %#018.16llx\n", acpi_memory_region_descriptor.length);
					printf_serial("acpi_memory_region_descriptor.type = %#010.8llx\n", acpi_memory_region_descriptor.type);
					printf_serial("acpi_memory_region_descriptor.attribute = %#010.8llx\n", acpi_memory_region_descriptor.attribute);
					rsdt_header = get_rsdt_header();
					PRINT_ACPI_TABLE_HEADER_P(rsdt_header);
					printf_serial("num_of_sdt_headers = %#010.8x\n", get_num_of_sdt_headers());
					print_sdts();
					fadt = get_fadt();
					PRINT_ACPI_TABLE_HEADER(fadt->header);
					printf_serial("fadt->firmware_ctrl = %p\n", fadt->firmware_ctrl);
					printf_serial("fadt->dsdt = %p\n", fadt->dsdt);
					printf_serial("fadt->reserved0 = %#04.2x\n", fadt->reserved0);
					printf_serial("fadt->preferred_pm_profile = %#04.2x\n", fadt->preferred_pm_profile);
					printf_serial("fadt->sci_int = %#06.4x\n", fadt->sci_int);
					printf_serial("fadt->smi_cmd = %#010.8x\n", fadt->smi_cmd);
					printf_serial("fadt->acpi_enable = %#04.2x\n", fadt->acpi_enable);
					printf_serial("fadt->acpi_disable = %#04.2x\n", fadt->acpi_disable);
					printf_serial("fadt->s4_bios_req = %#04.2x\n", fadt->s4_bios_req);
					printf_serial("fadt->pstate_cnt = %#04.2x\n", fadt->pstate_cnt);
					printf_serial("fadt->pm1a_evt_blk = %#010.8x\n", fadt->pm1a_evt_blk);
					printf_serial("fadt->pm1b_evt_blk = %#010.8x\n", fadt->pm1b_evt_blk);
					printf_serial("fadt->pm1a_cnt_blk = %#010.8x\n", fadt->pm1a_cnt_blk);
					if(!fadt->pm1a_cnt_blk)
					{
						ERROR(); // There is no valid address of pm1a_cnt_blk
						break;
					}
					printf_serial("fadt->pm1b_cnt_blk = %#010.8x\n", fadt->pm1b_cnt_blk);
					printf_serial("fadt->pm2_cnt_blk = %#010.8x\n", fadt->pm2_cnt_blk);
					printf_serial("fadt->pm_tmr_blk = %#010.8x\n", fadt->pm_tmr_blk);
					printf_serial("fadt->gpe0_blk = %#010.8x\n", fadt->gpe0_blk);
					printf_serial("fadt->gpe1_blk = %#010.8x\n", fadt->gpe1_blk);
					printf_serial("fadt->pm1_evt_len = %#04.2x\n", fadt->pm1_evt_len);
					printf_serial("fadt->pm1_cnt_len = %#04.2x\n", fadt->pm1_cnt_len);
					printf_serial("fadt->pm2_cnt_len = %#04.2x\n", fadt->pm2_cnt_len);
					printf_serial("fadt->pm_tmr_len = %#04.2x\n", fadt->pm_tmr_len);
					printf_serial("fadt->gpe0_blk_len = %#04.2x\n", fadt->gpe0_blk_len);
					printf_serial("fadt->gpe1_blk_len = %#04.2x\n", fadt->gpe1_blk_len);
					printf_serial("fadt->gpe1_base = %#04.2x\n", fadt->gpe1_base);
					printf_serial("fadt->cst_cnt = %#04.2x\n", fadt->cst_cnt);
					printf_serial("fadt->p_lvl2_lat = %#06.4x\n", fadt->p_lvl2_lat);
					printf_serial("fadt->p_lvl3_lat = %#06.4x\n", fadt->p_lvl3_lat);
					printf_serial("fadt->flush_size = %#06.4x\n", fadt->flush_size);
					printf_serial("fadt->flush_stride = %#06.4x\n", fadt->flush_stride);
					printf_serial("fadt->duty_offset = %#04.2x\n", fadt->duty_offset);
					printf_serial("fadt->duty_width = %#04.2x\n", fadt->duty_width);
					printf_serial("fadt->day_alrm = %#04.2x\n", fadt->day_alrm);
					printf_serial("fadt->mon_alarm = %#04.2x\n", fadt->mon_alrm);
					printf_serial("fadt->century = %#04.2x\n", fadt->century);
					printf_serial("fadt->iapc_boot_arch = %#06.4x\n", fadt->iapc_boot_arch);
					printf_serial("fadt->reserved1 = %#04.2x\n", fadt->reserved1);
					printf_serial("fadt->flags = %#010.8x\n", fadt->flags);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->reset_reg);
					printf_serial("fadt->reset_value = %#04.2x\n", fadt->reset_value);
					printf_serial("fadt->arm_boot_arch = %#06.4x\n", fadt->arm_boot_arch);
					printf_serial("fadt->fadt_minor_version = %#04.2x\n", fadt->fadt_minor_version);
					printf_serial("fadt->x_firmware_ctrl = %#018.16llx\n", fadt->x_firmware_ctrl);
					printf_serial("fadt->x_dsdt = %#018.16llx\n", fadt->x_dsdt);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm1a_evt_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm1b_evt_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm1a_cnt_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm1b_cnt_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm2_cnt_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm_tmr_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm_gpe0_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->x_pm_gpe1_blk);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->sleep_control_reg);
					PRINT_GENERIC_ADDRESS_STRUCTURE(fadt->sleep_status_reg);
					printf_serial("fadt->hypervisor_vender_identity = %#018.16llx\n", fadt->hypervisor_vender_identity);
					dsdt_header = get_dsdt_header();
					PRINT_ACPI_TABLE_HEADER_P(dsdt_header);
					dsdt_aml = get_dsdt_aml();
					printf_serial("---------- DSDT AML SYNTAX TREE ----------\n");
					dsdt_aml_syntax_tree = create_dsdt_aml_syntax_tree();
					print_aml_symbol(dsdt_aml_syntax_tree);
					printf_serial("dsdt_aml.length = %#010.8x\n", dsdt_aml.length);
					printf_serial("number of read bytes = %#010.8x\n", dsdt_aml_syntax_tree->string.length);
					printf_serial("---------- read bytes ----------\n");
					for(unsigned int i = 0; i < dsdt_aml_syntax_tree->string.length; i++)printf_serial("%02.2x%c", dsdt_aml_syntax_tree->string.initial[i], (i + 1) % 0x10 ? ' ' : '\n');
					printf_serial("\n");
					if(dsdt_aml_syntax_tree->string.length == dsdt_aml.length)
					{
						unsigned short pm1_cnt_slp_typ = get_aml_s5_pm1_cnt_slp_typ(dsdt_aml_syntax_tree);
						unsigned short pm1a_cnt_slp_typ = pm1_cnt_slp_typ & 0x00ff;
						unsigned short pm1b_cnt_slp_typ = pm1_cnt_slp_typ >> 8;
						printf_serial("pm1a_cnt_slp_typ = %#06.4x\n", pm1a_cnt_slp_typ);
						printf_serial("pm1b_cnt_slp_typ = %#06.4x\n", pm1b_cnt_slp_typ);
						// Shutdown command
						outw(fadt->pm1a_cnt_blk, pm1a_cnt_slp_typ << 10 | 0x2000);
						if(fadt->pm1b_cnt_blk)outw(fadt->pm1b_cnt_blk, pm1b_cnt_slp_typ << 10 | 0x2000);
						// Shutdown wait
						while(true);
					}
					else
					{
						printf_serial("next bytes =");
						for(unsigned char const *aml_byte = dsdt_aml_syntax_tree->string.initial + dsdt_aml_syntax_tree->string.length; aml_byte != dsdt_aml_syntax_tree->string.initial + dsdt_aml_syntax_tree->string.length + 0x10; aml_byte++)printf_serial(" %02.2x", *aml_byte);
						printf_serial("\n");
					}
					delete_aml_symbol(dsdt_aml_syntax_tree);
					printf_serial("DSDT AML syntax tree is deleted.\n");
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
				ApplicationTimer *application_timer;
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
				case TIMER_COMMAND_CREATE:
					application_timer = malloc(sizeof(*application_timer));
					application_timer->timer = create_timer(command->arguments.create.estimated_count, command->arguments.create.interval_count, task->event_queue, (void *(*)(void *))application_timer_procedure, (void *)application_timer, NULL);
					application_timer->owner_task = task;
					if(application_timers)
					{
						application_timer->previous = application_timers->previous;
						application_timer->next = application_timers;
						application_timers->previous->next = application_timer;
						application_timers->previous = application_timer;
					}
					else
					{
						application_timer->previous = application_timer;
						application_timer->next = application_timer;
						application_timers = application_timer;
					}
					file_descriptor->buffer_begin = malloc(sizeof(application_timer->timer));
					*(Timer **)file_descriptor->buffer_begin = application_timer->timer;
					file_descriptor->buffer_cursor = file_descriptor->buffer_begin;
					file_descriptor->buffer_end = (void *)((size_t)file_descriptor->buffer_begin + sizeof(application_timer->timer));
					break;
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
					window = create_window(command->arguments.create.title + application_memory, background_sheet, command->arguments.create.x, command->arguments.create.y, command->arguments.create.width + 2 * EDGE_WIDTH, command->arguments.create.height + TITLE_SHEET_HEIGHT + 3 * EDGE_WIDTH, task->event_queue);
					application_window = malloc(sizeof(*application_window));
					application_window->window = window;
					application_window->owner_task = task;
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
							if(0 <= x && x < command->arguments.draw_line.window->client_sheet->width && 0 <= y && y < command->arguments.draw_line.window->client_sheet->height)put_dot_sheet(command->arguments.draw_line.window->client_sheet, x, y, command->arguments.draw_line.color);
						}
						else for(short x = left_x; x <= right_x; x++)
						{
							short y = top_x == bottom_x ? top_y : ((top_y - bottom_y) * x + top_x * bottom_y - bottom_x * top_y) / (top_x - bottom_x);
							if(0 <= x && x < command->arguments.draw_line.window->client_sheet->width && 0 <= y && y < command->arguments.draw_line.window->client_sheet->height)put_dot_sheet(command->arguments.draw_line.window->client_sheet, x, y, command->arguments.draw_line.color);
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
					event = dequeue(task->event_queue);
					if(event)
					{
						switch(event->type)
						{
						case EVENT_TYPE_TIMER_EVENT:
							call_timer_procedure(event->event_union.timer_event.timer);
							break;
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
						case EVENT_TYPE_SHEET_CLICKED:
							application_window = get_application_window_from_window(get_window_from_sheet(event->event_union.sheet_clicked_event.sheet));
							if(application_window && application_window->window->client_sheet == event->event_union.sheet_clicked_event.sheet)
							{
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_CLICKED;
								new_application_event.event_union.window_clicked_event.window = application_window->window;
								new_application_event.event_union.window_clicked_event.x = event->event_union.sheet_clicked_event.x;
								new_application_event.event_union.window_clicked_event.y = event->event_union.sheet_clicked_event.y;
								new_application_event.event_union.window_clicked_event.flags = event->event_union.sheet_clicked_event.flags;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
							break;
						case EVENT_TYPE_SHEET_CREATED:
							application_window = get_application_window_from_window(get_window_from_sheet(event->event_union.sheet_created_event.sheet));
							if(application_window && application_window->window->client_sheet == event->event_union.sheet_created_event.sheet)
							{
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_CREATED;
								new_application_event.event_union.window_created_event.window = application_window->window;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
							break;
						case EVENT_TYPE_SHEET_KEYBOARD:
							application_window = get_application_window_from_window(get_window_from_sheet(event->event_union.sheet_keyboard_event.sheet));
							if(application_window && application_window->window->client_sheet == event->event_union.sheet_keyboard_event.sheet)
							{
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_KEYBOARD;
								new_application_event.event_union.window_keyboard_event.window = application_window->window;
								new_application_event.event_union.window_keyboard_event.keyboard_event = event->event_union.sheet_keyboard_event.keyboard_event;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
							break;
						case EVENT_TYPE_SHEET_MOUSE_DRAG:
							application_window = get_application_window_from_window(get_window_from_sheet(event->event_union.sheet_mouse_drag_event.sheet));
							if(application_window && application_window->window->client_sheet == event->event_union.sheet_mouse_drag_event.sheet)
							{
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_DRAG;
								new_application_event.event_union.window_drag_event.window = application_window->window;
								new_application_event.event_union.window_drag_event.x_movement = event->event_union.sheet_mouse_drag_event.x_movement;
								new_application_event.event_union.window_drag_event.y_movement = event->event_union.sheet_mouse_drag_event.y_movement;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
							break;
						case EVENT_TYPE_SHEET_MOUSE_MOVE:
							application_window = get_application_window_from_window(get_window_from_sheet(event->event_union.sheet_mouse_move_event.sheet));
							if(application_window && application_window->window->client_sheet == event->event_union.sheet_mouse_move_event.sheet)
							{
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_MOVE;
								new_application_event.event_union.window_move_event.window = application_window->window;
								new_application_event.event_union.window_move_event.x_movement = event->event_union.sheet_mouse_move_event.x_movement;
								new_application_event.event_union.window_move_event.y_movement = event->event_union.sheet_mouse_move_event.y_movement;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
							break;
						case EVENT_TYPE_SHEET_VERTICAL_WHEEL:
							application_window = get_application_window_from_window(get_window_from_sheet(event->event_union.sheet_vertical_wheel_event.sheet));
							if(application_window && application_window->window->client_sheet == event->event_union.sheet_vertical_wheel_event.sheet)
							{
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_VERTICAL_WHEEL;
								new_application_event.event_union.window_vertical_wheel_event.window = application_window->window;
								new_application_event.event_union.window_vertical_wheel_event.rotation = event->event_union.sheet_vertical_wheel_event.rotation;
								new_application_event.event_union.window_vertical_wheel_event.x = event->event_union.sheet_vertical_wheel_event.x;
								new_application_event.event_union.window_vertical_wheel_event.y = event->event_union.sheet_vertical_wheel_event.y;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
							break;
						case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
							application_window = get_application_window_from_window(event->event_union.window_deletion_response_event.window);
							if(application_window)
							{
								if(application_window == application_windows)application_windows = application_window->next;
								if(application_window == application_windows)application_windows = NULL;
								application_window->previous->next = application_window->next;
								application_window->next->previous = application_window->previous;
								free(application_window);
								new_application_event.type = APPLICATION_EVENT_TYPE_WINDOW_DELETION_RESPONSE;
								new_application_event.event_union.window_deletion_response_event.window = event->event_union.window_deletion_response_event.window;
								enqueue(system_call_status->application_event_queue, &new_application_event);
							}
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

