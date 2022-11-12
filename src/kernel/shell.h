#ifndef _SHELL_H_
#define _SHELL_H_

struct _ChainString;
struct _Queue;
struct _Shell;
struct _Task;
struct _TaskReturn;

typedef struct _ShellPutCharacterEvent
{
	struct _Shell *shell;
	char character;
} ShellPutCharacterEvent;

typedef struct _DictionaryElement
{
	char *key;
	char *value;
	struct _DictionaryElement *previous;
	struct _DictionaryElement *next;
} DictionaryElement;

typedef struct _Dictionary
{
	DictionaryElement *elements;
} Dictionary;

typedef struct _Redirection
{
	struct _Shell *shell;
	struct _Task *task;
	char *destination_file_name;
	struct _ChainString *output;
	struct _Redirection *previous;
	struct _Redirection *next;
} Redirection;

typedef struct _Shell
{
	struct _Console *console;
	struct _Queue *event_queue;
	Dictionary *variables;
	Redirection *redirections;
	struct _Shell *previous;
	struct _Shell *next;
	unsigned char type;
	#define SHELL_TYPE_CONSOLE	0x00
	#define SHELL_TYPE_SERIAL	0x01
	unsigned char flags;
	#define SHELL_FLAG_BUSY		0x01
	#define SHELL_FLAG_EXIT_REQUEST	0x02
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
	int return_value;
} CommandTaskReturn;

#include "chain_string.h"
#include "console.h"
#include "task.h"

extern char const * const prompt;
extern ChainString *serial_console_input_string;
extern Shell *serial_shell;

void clean_up_command_task(Task *command_task, CommandTaskArgument *command_task_argument);
Redirection *create_redirection(Shell *shell, Task *task, char *destination_file_name);
Shell *create_shell(struct _Console *console);
void delete_redirection(Task *command_task);
void delete_shell(Shell *shell);
void *execute_command(Shell *shell, char const *command);
Shell *get_current_shell(void);
Redirection *get_redirection(Task *task);
void init_shells(void);
void print_shell(Shell *shell, char const *string);
void printf_shell(Shell *shell, char const *format, ...);
void put_char_redirection(Redirection *redirection, char character);
void put_char_shell(Shell *shell, char character);

#endif

