#include "console.h"
#include "gdt.h"
#include "memory.h"
#include "serial.h"
#include "string.h"

Console *root_console = NULL;

void delete_console(Console *console);
Console *get_console_from_sheet(Sheet const *sheet);

void *console_event_procedure(Sheet *sheet, struct _Event const *event)
{
	bool prompt_position_exists;
	Console *console = get_console_from_sheet(sheet);
	ConsoleEvent *console_event;
	Event new_event;
	void *return_value;
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_CREATED:
		return_value = console->default_event_procedure(sheet, event);
		// Send the first prompt event.
		console_event = malloc(sizeof(*console_event));
		console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
		new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
		new_event.event_union.sheet_user_defined_event.sheet = sheet;
		new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
		new_event.event_union.sheet_user_defined_event.any = console_event;
		enqueue(sheet->event_queue, &new_event);
		return return_value;
	case EVENT_TYPE_SHEET_DELETION_REQUEST:
		delete_console(console);
		return console->default_event_procedure(sheet, event);
	case EVENT_TYPE_SHEET_KEYBOARD:
		return_value = console->default_event_procedure(sheet, event);
		// Check that there is the prompt position in the text.
		prompt_position_exists = false;
		for(CharacterPosition *character_position = console->text_box->last_position; character_position; character_position = character_position->previous)if(character_position == console->prompt_position)
		{
			prompt_position_exists = true;
			break;
		}
		if(!prompt_position_exists)console->prompt_position = NULL;
		if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_KEY_PUSHED)switch(event->event_union.keyboard_event.character)
		{
		case '\n':
			if(!console->text_box->cursor_position)
			{
				if(console->prompt_position)
				{
					// Send command issued event.
					ChainCharacter *command_start_point;
					ChainString *command;
					command_start_point = console->prompt_position->character;
					for(unsigned int i = 0; i < strlen(prompt); i++)command_start_point = command_start_point->next;
					if(command_start_point && command_start_point != console->text_box->string->last_character)
					{
						command = create_chain_substring(command_start_point, console->text_box->string->last_character->previous);
						console_event = malloc(sizeof(*console_event));
						console_event->type = CONSOLE_EVENT_TYPE_COMMAND_ISSUED;
						console_event->console_event_union.command_issued_event.command = create_char_array_from_chain_string(command);
						delete_chain_string(command);
						new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
						new_event.event_union.sheet_user_defined_event.sheet = sheet;
						new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
						new_event.event_union.sheet_user_defined_event.any = console_event;
						enqueue(sheet->event_queue, &new_event);
					}
					else
					{
						// Send prompt event.
						console_event = malloc(sizeof(*console_event));
						console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
						new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
						new_event.event_union.sheet_user_defined_event.sheet = sheet;
						new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
						new_event.event_union.sheet_user_defined_event.any = console_event;
						enqueue(sheet->event_queue, &new_event);
					}
				}
				else
				{
					// Send prompt event.
					console_event = malloc(sizeof(*console_event));
					console_event->type = CONSOLE_EVENT_TYPE_PROMPT;
					new_event.type = EVENT_TYPE_SHEET_USER_DEFINED;
					new_event.event_union.sheet_user_defined_event.sheet = sheet;
					new_event.event_union.sheet_user_defined_event.procedure = console_event_procedure;
					new_event.event_union.sheet_user_defined_event.any = console_event;
					enqueue(sheet->event_queue, &new_event);
				}
			}
			break;
		}
		return return_value;
	case EVENT_TYPE_SHEET_USER_DEFINED:
		if(event->event_union.sheet_user_defined_event.procedure == console_event_procedure)
		{
			console_event = (ConsoleEvent *)event->event_union.sheet_user_defined_event.any;
			switch(console_event->type)
			{
			case CONSOLE_EVENT_TYPE_COMMAND_ISSUED:
				// Execute the command.
				execute_command(console->shell, console_event->console_event_union.command_issued_event.command);
				free(console_event->console_event_union.command_issued_event.command);
				break;
			case CONSOLE_EVENT_TYPE_PROMPT:
				// Print prompt.
				console->prompt_position = console->text_box->last_position;
				text_box_insert_string_back(console->text_box, console->text_box->last_position, prompt);
				if(console->prompt_position)console->prompt_position = console->prompt_position->next;
				else console->prompt_position = console->text_box->first_position;
				break;
			default:
				ERROR();
				break;
			}
			free(console_event);
			return NULL;
		}
		else return console->default_event_procedure(sheet, event);
	default:
		return console->default_event_procedure(sheet, event);
	}
}

void console_task_procedure(ConsoleTaskArgument *console_task_argument)
{
				//{red ,green, blue,alpha}
	Color background_color	= {0x00, 0x00, 0x00, 0xff};
	Color foreground_color	= {0xff, 0xff, 0xff, 0xff};
	Queue *event_queue;
	Task *task;
	TaskReturn *task_return;
	Window *window;
	printf_serial("Hello, Console Task!\n");
	task = get_current_task();
	event_queue = create_event_queue(task);
	window = create_window("Console", console_task_argument->background_sheet, 8 * task->segment_selector, 8 * task->segment_selector, 0x0200, 0x0200, event_queue);
	make_sheet_console(window->client_sheet, foreground_color, background_color);
	while(true)
	{
		Event new_event;
		Event const *event = dequeue(event_queue);
		if(event)switch(event->type)
		{
		case EVENT_TYPE_SHELL_PUT_CHARACTER:
			put_char_shell(event->event_union.shell_put_character_event.shell, event->event_union.shell_put_character_event.character);
			break;
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
		case EVENT_TYPE_WINDOW_FOCUSED:
		case EVENT_TYPE_WINDOW_UNFOCUSED:
			distribute_event(event);
			break;
		case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			distribute_event(event);
			if(event->event_union.window_deletion_response_event.window == window)
			{
				new_event.type = EVENT_TYPE_TASK_DELETION_REQUEST;
				new_event.event_union.task_deletion_request_event.task = task;
				enqueue(event_queue, &new_event);
				printf_serial("Enqueue console task deletion request!\n");
			}
			break;
		case EVENT_TYPE_TASK_DELETION_REQUEST:
			printf_serial("Detect console task deletion request.\n");
			close_task(task);
			ERROR(); // Can't close task!
			break;
		case EVENT_TYPE_TASK_DELETION_RESPONSE:
			task_return = (TaskReturn*)event->event_union.task_deletion_response_event.returns;
			switch(task_return->task_type)
			{
			case TASK_TYPE_COMMAND:
				clean_up_command_task(event->event_union.task_deletion_response_event.arguments);
				break;
			default:
				ERROR(); // Invalid task type
				break;
			}
			free(event->event_union.task_deletion_response_event.returns);
			free(event->event_union.task_deletion_response_event.arguments);
			free_segment(event->event_union.task_deletion_response_event.segment_selector);
			printf_serial("free_segment %#06x\n", event->event_union.task_deletion_response_event.segment_selector);
			break;
		default: // invalid event->type
			ERROR();
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else sleep_task(task);
	}
}

void delete_console(Console *console)
{
	printf_serial("Delete console %p\n", console);
	prohibit_switch_task();
	delete_shell(console->shell);
	console->text_box->sheet->event_procedure = console->default_event_procedure;
	if(root_console == console)root_console = root_console->next;
	if(root_console == console)root_console = NULL;
	console->previous->next = console->next;
	console->next->previous = console->previous;
	free(console);
	allow_switch_task();
}

Console *get_console_from_sheet(Sheet const *sheet)
{
	Console *candidate_console;
	prohibit_switch_task();
	candidate_console = root_console;
	do
	{
		if(candidate_console->text_box->sheet == sheet)
		{
			allow_switch_task();
			return candidate_console;
		}
		candidate_console = candidate_console->next;
	} while(candidate_console != root_console);
	ERROR(); // Console is not found.
	allow_switch_task();
	return NULL;
}

Console *make_sheet_console(Sheet *sheet, Color foreground_color, Color background_color)
{
	Console *new_console = malloc(sizeof(*new_console));
	printf_serial("Make sheet %p console %p\n", sheet, new_console);
	prohibit_switch_task();
	new_console->prompt_position = NULL;
	new_console->shell = create_shell(new_console);
	new_console->text_box = make_sheet_text_box(sheet, foreground_color, background_color);
	new_console->default_event_procedure = new_console->text_box->sheet->event_procedure;
	new_console->text_box->sheet->event_procedure = console_event_procedure;
	if(root_console)
	{
		new_console->next = root_console;
		new_console->previous = root_console->previous;
		root_console->previous->next = new_console;
		root_console->previous = new_console;
	}
	else
	{
		new_console->next = new_console;
		new_console->previous = new_console;
		root_console = new_console;
	}
	allow_switch_task();
	return new_console;
}

