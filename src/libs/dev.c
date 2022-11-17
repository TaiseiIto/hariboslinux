#include "chain_string.h"
#include "dev.h"
#include "stdio.h"
#include "stdlib.h"

typedef struct _AMLSubstring
{
	unsigned char const *initial;
	size_t length;
} AMLSubstring;

typedef struct _ACPICommandDecodeAML
{
	AMLSubstring aml;
} ACPICommandDecodeAML;

typedef union _ACPICommandArguments
{
	ACPICommandDecodeAML decode_aml;
} ACPICommandArguments;

typedef struct _ACPICommand
{
	ACPICommandArguments arguments;
	unsigned char type;
	#define ACPI_COMMAND_DECODE_AML	0x00
} ACPICommand;

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

typedef struct _DiskCommand
{
	unsigned char type;
	#define DISK_COMMAND_SAVE	0x00
} DiskCommand;

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
	unsigned int window;
	short x1;
	short y1;
	short x2;
	short y2;
	Color color;
} WindowCommandDrawLine;

typedef struct _WindowCommandFillBox
{
	unsigned int window;
	short x;
	short y;
	unsigned short width;
	unsigned short height;
	Color color;
} WindowCommandFillBox;

typedef struct _WindowCommandPrint
{
	unsigned int window;
	short x;
	short y;
	Color foreground;
	Color background;
	char const *string;
} WindowCommandPrint;

typedef struct _WindowCommandPutDot
{
	unsigned int window;
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

char const * const acpi_file_name = "acpi.dev";
char const * const console_file_name = "console.dev";
char const * const cpu_file_name = "cpu.dev";
char const * const disk_file_name = "disk.dev";
char const * const memory_file_name = "memory.dev";
char const * const timer_file_name = "timer.dev";
char const * const window_file_name = "window.dev";

unsigned int acpi_file = 0;
unsigned int console_file = 0;
unsigned int cpu_file = 0;
unsigned int disk_file = 0;
unsigned int memory_file = 0;
unsigned int timer_file = 0;
unsigned int window_file = 0;

void clear_console(void)
{
	ConsoleCommand command;
	if(!console_file)console_file = fopen(console_file_name, "wr");
	command.type = CONSOLE_COMMAND_CLEAR;
	fwrite(&command, sizeof(command), 1, console_file);
}

unsigned int create_timer(unsigned long long estimated_count, unsigned long long interval_count)
{
	unsigned int timer;
	TimerCommand command;
	if(!timer_file)timer_file = fopen(timer_file_name, "wr");
	command.type = TIMER_COMMAND_CREATE;
	command.arguments.create.estimated_count = estimated_count;
	command.arguments.create.interval_count = interval_count;
	fwrite(&command, sizeof(command), 1, timer_file);
	fread(&timer, sizeof(timer), 1, timer_file);
	return timer;
}

unsigned int create_window(char const *title, short x, short y, unsigned short width, unsigned short height)
{
	unsigned int window;
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_CREATE;
	command.arguments.create.title = title;
	command.arguments.create.x = x;
	command.arguments.create.y = y;
	command.arguments.create.width = width;
	command.arguments.create.height = height;
	fwrite(&command, sizeof(command), 1, window_file);
	fread(&window, sizeof(window), 1, window_file);
	return window;
}

void decode_aml(unsigned char const *aml, size_t length)
{
	ACPICommand command;
	if(!acpi_file)acpi_file = fopen(acpi_file_name, "w");
	command.type = ACPI_COMMAND_DECODE_AML;
	command.arguments.decode_aml.aml.initial = aml;
	command.arguments.decode_aml.aml.length = length;
	fwrite(&command, sizeof(command), 1, acpi_file);
}

ApplicationEvent dequeue_application_event(void)
{
	ApplicationEvent application_event;
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_DEQUEUE_EVENT;
	fwrite(&command, sizeof(command), 1, window_file);
	fread(&application_event, sizeof(application_event), 1, window_file);
	return application_event;
}

void draw_line_window(unsigned int window, short x1, short y1, short x2, short y2, Color color)
{
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_DRAW_LINE;
	command.arguments.draw_line.window = window;
	command.arguments.draw_line.x1 = x1;
	command.arguments.draw_line.y1 = y1;
	command.arguments.draw_line.x2 = x2;
	command.arguments.draw_line.y2 = y2;
	command.arguments.draw_line.color = color;
	fwrite(&command, sizeof(command), 1, window_file);
}

void exit_console(void)
{
	ConsoleCommand command;
	if(!console_file)console_file = fopen(console_file_name, "wr");
	command.type = CONSOLE_COMMAND_EXIT;
	fwrite(&command, sizeof(command), 1, console_file);
}

void fill_box_window(unsigned int window, short x, short y, unsigned short width, unsigned short height, Color color)
{
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_FILL_BOX;
	command.arguments.fill_box.window = window;
	command.arguments.fill_box.x = x;
	command.arguments.fill_box.y = y;
	command.arguments.fill_box.width = width;
	command.arguments.fill_box.height = height;
	command.arguments.fill_box.color = color;
	fwrite(&command, sizeof(command), 1, window_file);
}

unsigned int get_free_memory_space_size(void)
{
	MemoryCommand command;
	unsigned int free_memory_space_size;
	if(!memory_file)memory_file = fopen(memory_file_name, "wr");
	command.type = MEMORY_COMMAND_FREE;
	fwrite(&command, sizeof(command), 1, memory_file);
	fread(&free_memory_space_size, sizeof(free_memory_space_size), 1, memory_file);
	return free_memory_space_size;
}

unsigned int get_unix_time(void)
{
	TimerCommand command;
	unsigned int unix_time;
	if(!timer_file)timer_file = fopen(timer_file_name, "wr");
	command.type = TIMER_COMMAND_GET;
	fwrite(&command, sizeof(command), 1, timer_file);
	fread(&unix_time, sizeof(unix_time), 1, timer_file);
	return unix_time;
}

void hlt_application(void)
{
	CPUCommand command;
	if(!cpu_file)cpu_file = fopen(cpu_file_name, "wr");
	command.type = CPU_COMMAND_HLT;
	fwrite(&command, sizeof(command), 1, cpu_file);
}

void print_window(unsigned int window, short x, short y, Color foreground, Color background, char const *string)
{
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_PRINT;
	command.arguments.print.window = window;
	command.arguments.print.x = x;
	command.arguments.print.y = y;
	command.arguments.print.foreground = foreground;
	command.arguments.print.background = background;
	command.arguments.print.string = string;
	fwrite(&command, sizeof(command), 1, window_file);
}

void printf_window(unsigned int window, short x, short y, Color foreground, Color background, char const *format, ...)
{
	ChainString *output_chain_string = create_caller_format_chain_string(((unsigned int)&format - (unsigned int)&window) / sizeof(unsigned int));
	char *output_string = create_char_array_from_chain_string(output_chain_string);
	print_window(window, x, y, foreground, background, output_string);
	free(output_string);
	delete_chain_string(output_chain_string);
}

void process_event(void)
{
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_PROCESS_EVENT;
	fwrite(&command, sizeof(command), 1, window_file);
}

void put_dot_window(unsigned int window, unsigned short x, unsigned short y, Color color)
{
	WindowCommand command;
	if(!window_file)window_file = fopen(window_file_name, "wr");
	command.type = WINDOW_COMMAND_PUT_DOT;
	command.arguments.put_dot.window = window;
	command.arguments.put_dot.x = x;
	command.arguments.put_dot.y = y;
	command.arguments.put_dot.color = color;
	fwrite(&command, sizeof(command), 1, window_file);
}

void save_disk(void)
{
	DiskCommand command;
	if(!disk_file)disk_file = fopen(disk_file_name, "wr");
	command.type = DISK_COMMAND_SAVE;
	fwrite(&command, sizeof(command), 1, disk_file);
}

void shutdown(void)
{
	CPUCommand command;
	if(!cpu_file)cpu_file = fopen(cpu_file_name, "wr");
	command.type = CPU_COMMAND_SHUTDOWN;
	fwrite(&command, sizeof(command), 1, cpu_file);
}

