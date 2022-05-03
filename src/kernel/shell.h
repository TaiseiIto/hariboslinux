#ifndef _SHELL_H_
#define _SHELL_H_

struct _Queue;
struct _Shell;
struct _TaskReturn;

typedef struct _ShellPutCharacterEvent
{
	struct _Shell *shell;
	char character;
} ShellPutCharacterEvent;

typedef struct _Shell
{
	struct _Queue *event_queue;
	struct _Console *console;
	struct _Shell *previous;
	struct _Shell *next;
	unsigned char type;
	#define SHELL_TYPE_CONSOLE	0x00
	#define SHELL_TYPE_SERIAL	0x01
} Shell;

typedef struct _CommandTaskAdditional
{
	void *application_memory;
} CommandTaskAdditional;

typedef struct _CommandTaskArgument
{
	char *com_file_name;
	void *com_file_binary;
	unsigned int com_file_size;
	unsigned int argc;
	char **argv;
	Shell *shell;
	struct _TaskReturn *task_return;
} CommandTaskArgument;

typedef struct _CommandTaskReturn
{
	unsigned int return_value;
} CommandTaskReturn;

#include "chain_string.h"
#include "console.h"
#include "task.h"

extern char const * const prompt;
extern ChainString *serial_console_input_string;
extern Shell *serial_shell;

void clean_up_command_task(CommandTaskArgument *command_task_argument);
Shell *create_shell(struct _Console *console);
void delete_shell(Shell *shell);
void *execute_command(Shell *shell, char const *command);
Shell *get_current_shell(void);
void init_shells(void);
void print_shell(Shell *shell, char const *string);
void printf_shell(Shell *shell, char const *format, ...);
void put_char_shell(Shell *shell, char character);

#endif

