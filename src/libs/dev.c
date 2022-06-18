#include "dev.h"
#include "stdio.h"

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

typedef struct _WindowCommandDequeueEvent
{
	unsigned int window;
} WindowCommandDequeueEvent;

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
	WindowCommandCreateArguments create;
	WindowCommandDequeueEvent dequeue_event;
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
	#define WINDOW_COMMAND_PUT_DOT		0x05
} WindowCommand;

char const * const console_file_name = "console.dev";
char const * const memory_file_name = "memory.dev";
char const * const timer_file_name = "timer.dev";
char const * const window_file_name = "window.dev";

void clear_console(void)
{
	ConsoleCommand command;
	unsigned int file_descriptor = fopen(console_file_name, "w");
	command.type = CONSOLE_COMMAND_CLEAR;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
}

unsigned int create_window(char const *title, short x, short y, unsigned short width, unsigned short height)
{
	unsigned int file_descriptor = fopen(window_file_name, "wr");
	unsigned int window;
	WindowCommand command;
	command.type = WINDOW_COMMAND_CREATE;
	command.arguments.create.title = title;
	command.arguments.create.x = x;
	command.arguments.create.y = y;
	command.arguments.create.width = width;
	command.arguments.create.height = height;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fread(&window, sizeof(window), 1, file_descriptor);
	fclose(file_descriptor);
	return window;
}

ApplicationEvent dequeue_application_event(unsigned int window)
{
	ApplicationEvent application_event;
	unsigned int file_descriptor = fopen(window_file_name, "wr");
	WindowCommand command;
	command.type = WINDOW_COMMAND_DEQUEUE_EVENT;
	command.arguments.dequeue_event.window = window;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fread(&application_event, sizeof(application_event), 1, file_descriptor);
	fclose(file_descriptor);
	return application_event;
}

void draw_line_window(unsigned int window, short x1, short y1, short x2, short y2, Color color)
{
	unsigned int file_descriptor = fopen(window_file_name, "w");
	WindowCommand command;
	command.type = WINDOW_COMMAND_DRAW_LINE;
	command.arguments.draw_line.window = window;
	command.arguments.draw_line.x1 = x1;
	command.arguments.draw_line.y1 = y1;
	command.arguments.draw_line.x2 = x2;
	command.arguments.draw_line.y2 = y2;
	command.arguments.draw_line.color = color;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
}

void fill_box_window(unsigned int window, short x, short y, unsigned short width, unsigned short height, Color color)
{
	unsigned int file_descriptor = fopen(window_file_name, "w");
	WindowCommand command;
	command.type = WINDOW_COMMAND_FILL_BOX;
	command.arguments.fill_box.window = window;
	command.arguments.fill_box.x = x;
	command.arguments.fill_box.y = y;
	command.arguments.fill_box.width = width;
	command.arguments.fill_box.height = height;
	command.arguments.fill_box.color = color;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
}

unsigned int get_free_memory_space_size(void)
{
	MemoryCommand command;
	unsigned int file_descriptor = fopen(memory_file_name, "wr");
	unsigned int free_memory_space_size;
	command.type = MEMORY_COMMAND_FREE;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fread(&free_memory_space_size, sizeof(free_memory_space_size), 1, file_descriptor);
	fclose(file_descriptor);
	return free_memory_space_size;
}

unsigned int get_unix_time(void)
{
	TimerCommand command;
	unsigned int file_descriptor = fopen(timer_file_name, "wr");
	unsigned int unix_time;
	command.type = TIMER_COMMAND_GET;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fread(&unix_time, sizeof(unix_time), 1, file_descriptor);
	fclose(file_descriptor);
	return unix_time;
}

void print_window(unsigned int window, short x, short y, Color foreground, Color background, char const *string)
{
	unsigned int file_descriptor = fopen(window_file_name, "w");
	WindowCommand command;
	command.type = WINDOW_COMMAND_PRINT;
	command.arguments.print.window = window;
	command.arguments.print.x = x;
	command.arguments.print.y = y;
	command.arguments.print.foreground = foreground;
	command.arguments.print.background = background;
	command.arguments.print.string = string;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
}

void put_dot_window(unsigned int window, unsigned short x, unsigned short y, Color color)
{
	unsigned int file_descriptor = fopen(window_file_name, "w");
	WindowCommand command;
	command.type = WINDOW_COMMAND_PUT_DOT;
	command.arguments.put_dot.window = window;
	command.arguments.put_dot.x = x;
	command.arguments.put_dot.y = y;
	command.arguments.put_dot.color = color;
	fwrite(&command, sizeof(command), 1, file_descriptor);
	fclose(file_descriptor);
}

