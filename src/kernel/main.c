#include "beep.h"
#include "boot.h"
#include "chain_string.h"
#include "console.h"
#include "disk.h"
#include "event.h"
#include "font.h"
#include "fpu.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "mouse.h"
#include "pic.h"
#include "pit.h"
#include "rtc.h"
#include "serial.h"
#include "sheet.h"
#include "stdio.h"
#include "string.h"
#include "task.h"
#include "text_box.h"
#include "timer.h"
#include "window.h"

typedef struct _TestTaskArgument
{
	Sheet *background_sheet;
} TestTaskArgument;

void *background_sheet_procedure(Sheet *sheet, struct _Event const *event);
void test_task_procedure(TestTaskArgument *test_task_argument);

int main(void)
{
	BootInformation const * const boot_information = (BootInformation const * const)MEMORY_MAP_BOOT_INFORMATION;
	Color background_color;
	Color foreground_color;
	MemoryRegionDescriptor memory_region_descriptor;
	Queue *event_queue;
	Sheet *background_sheet;
	Sheet *mouse_cursor_sheet;
	Task *main_task;
	Timer *checking_free_memory_space_size_timer;
	Timer *test_timer;
	Timer *serial_status_timer;
	unsigned int memory_region_descriptor_index;
	unsigned long long timer_counter = 0;
	unsigned short keyboard_flags = 0;
	unsigned short screen_text_row = 9;
	new_line_serial();
	print_serial("Hello, kernel.bin!\n\n");
	init_gdt();
	print_serial("finish init_gdt()\n\n");
	init_idt();
	print_serial("finish init_idt()\n\n");
	init_memory();
	print_serial("finish init_memory()\n\n");
	main_task = init_task();
	print_serial("finish init_task()\n\n");
	event_queue = create_event_queue(main_task);
	print_serial("finish create_queue()\n\n");
	init_pic();
	print_serial("finish init_pic()\n\n");
	init_pit(event_queue);
	print_serial("finish init_pit()\n\n");
	init_keyboard(event_queue, main_task);
	print_serial("finish init_keyboard()\n\n");
	init_rtc(event_queue);
	print_serial("finish init_rtc()\n\n");
	init_mouse(event_queue);
	print_serial("finish init_mouse()\n\n");
	init_screen();
	print_serial("finish init_screen()\n\n");
	init_fpu();
	print_serial("finish init_fpu()\n\n");
	init_serial_interrupt(main_task);
	sti_task();
	print_serial("finish init_serial_interrupt() and sti_task()\n\n");
	init_file_system();
	init_shells();
	// Deploy callbios.bin
	char const * const call_bios_bin_name = "callbios.bin";
	unsigned int call_bios_bin_size = get_file_size(call_bios_bin_name);
	void *call_bios_bin = load_file(call_bios_bin_name);
	memcpy(MEMORY_MAP_CALL_BIOS, call_bios_bin, call_bios_bin_size);
	free(call_bios_bin);
	// Init background sheet
	init_sheets(&background_sheet, background_sheet_procedure, &mouse_cursor_sheet, event_queue);
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	background_color.alpha = 0xff;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	foreground_color.alpha = 0xff;
	printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "mouse ID = %#04x", get_mouse_id());
	printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "keyboard state = %#04x", boot_information->keyboard_state);
	printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded cylinder = %#04x", boot_information->last_loaded_cylinder);
	printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded head = %#04x", boot_information->last_loaded_head);
	printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded sector = %#04x", boot_information->last_loaded_sector);
	printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "memory regions");
	// Print memory regions
	memory_region_descriptor_index = 0;
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "base = %#018.16llx, length = %#018.16llx, type = %#010.8x, attribute = %#010.8x\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		memory_region_descriptor_index++;
	} while(memory_region_descriptor.base != 0 || memory_region_descriptor.length != 0 || memory_region_descriptor.type != 0 || memory_region_descriptor.attribute != 0);
	checking_free_memory_space_size_timer = create_timer(0, 100, event_queue, NULL, NULL, NULL);
	// Test timer
	test_timer = create_timer(0, 100, event_queue, NULL, NULL, NULL);
	// Serial status timer
	serial_status_timer = create_timer(0, 100, event_queue, NULL, NULL, NULL);
	while(true)
	{
		Event const *event;
		event = dequeue(event_queue);
		if(event)switch(event->type)
		{
		case EVENT_TYPE_KEYBOARD_EVENT:
			if((keyboard_flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)) != (event->event_union.keyboard_event.flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)))
			{
				if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_ENGLISH)print_sheet(background_sheet, 0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = English ");
				else if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_JAPANESE)print_sheet(background_sheet, 0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = Japanese");
			}
			if(event->event_union.keyboard_event.character)printf_sheet(background_sheet, 0x0000, 0x0002 * CHAR_HEIGHT, foreground_color, background_color, "keyboard event character = %c", event->event_union.keyboard_event.character);
			keyboard_flags = event->event_union.keyboard_event.flags;
			if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_ALT_KEY_PUSHED && event->event_union.keyboard_event.flags & KEYBOARD_FLAG_KEY_PUSHED)switch(event->event_union.keyboard_event.keycode)
			{
			case KEY_TAB: // Switch window by ALT + TAB.
				{
					Sheet *pulled_up_sheet = background_sheet->lowest_child;
					Window *pulled_up_window = get_window_from_sheet(pulled_up_sheet);
					Sheet *next_focused_sheet = pulled_up_window ? pulled_up_window->client_sheet : pulled_up_sheet ;
					if(pulled_up_window)focus_window(pulled_up_window);
					pull_up_sheet(pulled_up_sheet);
					focus_sheet(next_focused_sheet);
				}
				break;
			case KEY_F4: // Delete window by ALT + F4.
				{
					Event new_event;
					Window *window = get_focused_window();
					if(window)
					{
						new_event.type = EVENT_TYPE_WINDOW_DELETION_REQUEST;
						new_event.event_union.window_deletion_request_event.window = window;
						enqueue(window->root_sheet->event_queue, &new_event);
					}
				}
				break;
			}
			else send_sheets_event(event);
			break;
		case EVENT_TYPE_KEYBOARD_INTERRUPT:
			printf_sheet(background_sheet, 0x0000, 0x0003 * CHAR_HEIGHT, foreground_color, background_color, "keyboard interrupt signal = %#04x", event->event_union.keyboard_interrupt.signal);
			decode_keyboard_interrupt(event->event_union.keyboard_interrupt.signal);
			break;
		case EVENT_TYPE_MOUSE_EVENT:
			printf_sheet(background_sheet, 0x0000, 0x0004 * CHAR_HEIGHT, foreground_color, background_color, "mouse state x=%4d, y=%4d, vertical wheel=%4d, horizontal wheel=%4d, left button=%c, middle button=%c, right button=%c, 4th button=%c, 5th button=%c", event->event_union.mouse_event.x, event->event_union.mouse_event.y, event->event_union.mouse_event.vertical_wheel_movement, event->event_union.mouse_event.horizontal_wheel_movement, (event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED) ? '1' : '0');
			send_sheets_event(event);
			break;
		case EVENT_TYPE_MOUSE_INTERRUPT:
			printf_sheet(background_sheet, 0x0000, 0x0005 * CHAR_HEIGHT, foreground_color, background_color, "mouse interrupt signal = %#04x", event->event_union.mouse_interrupt.signal);
			decode_mouse_interrupt(event->event_union.mouse_interrupt.signal);
			break;
		case EVENT_TYPE_PIT_INTERRUPT:
			decode_pit_interrupt();
			break;
		case EVENT_TYPE_RTC_INTERRUPT:
			printf_sheet(background_sheet, 0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "%04d/%02d/%02d %s %02d:%02d:%02d", event->event_union.rtc_interrupt.year, event->event_union.rtc_interrupt.month, event->event_union.rtc_interrupt.day, get_day_of_week_string(get_day_of_week(event->event_union.rtc_interrupt.year, event->event_union.rtc_interrupt.month, event->event_union.rtc_interrupt.day)), event->event_union.rtc_interrupt.hour, event->event_union.rtc_interrupt.minute, event->event_union.rtc_interrupt.second);
			break;
		case EVENT_TYPE_SERIAL_INTERRUPT:
			serial_console_input(event->event_union.serial_interrupt.data);
			break;
		case EVENT_TYPE_SHELL_PUT_CHARACTER:
			put_char_shell(event->event_union.shell_put_character_event.shell, event->event_union.shell_put_character_event.character);
			break;
		case EVENT_TYPE_TASK_DELETION_RESPONSE:
			background_sheet->event_procedure(background_sheet, event);
			break;
		case EVENT_TYPE_TIMER_EVENT:
			if(event->event_union.timer_event.timer == test_timer)
			{
				timer_counter++;
				printf_sheet(background_sheet, 0x0000, 0x0006 * CHAR_HEIGHT, foreground_color, background_color, "timer = %lld seconds", timer_counter);
			}
			else if(event->event_union.timer_event.timer == checking_free_memory_space_size_timer)
			{
				printf_sheet(background_sheet, 0x0000, 0x0007 * CHAR_HEIGHT, foreground_color, background_color, "free memory space size = %#010x bytes", get_free_memory_space_size());
			}
			else if(event->event_union.timer_event.timer == serial_status_timer)
			{
				printf_sheet(background_sheet, 0x0000, 0x0008 * CHAR_HEIGHT, foreground_color, background_color, "number of unoutput characters = %#010x", number_of_unoutput_characters());
			}
			else call_timer_procedure(event->event_union.timer_event.timer);
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
		default: // invalid event->type
			ERROR();
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else sleep_task(main_task);
	}
}

void *background_sheet_procedure(Sheet *sheet, struct _Event const *event)
{
	ChainString *chain_string;
	char *string;
	Color translucent_red;
	Color translucent_green;
	Color translucent_blue;
	Sheet *translucent_red_sheet;
	Sheet *translucent_green_sheet;
	Sheet *translucent_blue_sheet;
	Task *test_task;
	TestTaskArgument *test_task_argument;
	TaskReturn *task_return;
	Task *console_task;
	ConsoleTaskArgument *console_task_argument;
	TaskReturn *console_task_return;
	TaskReturn *test_task_return;
	switch(event->type)
	{
	case EVENT_TYPE_SHEET_KEYBOARD:
		printf_serial("Keyboard event @ background sheet.\n");
		switch(event->event_union.keyboard_event.keycode)
		{
		case KEY_A:
			// Chain string test
			chain_string = create_chain_string("Hello, chain string!\n");
			string = create_char_array_from_chain_string(chain_string);
			printf_serial(string);
			free(string);
			delete_chain_string(chain_string);
			break;
		case KEY_C:
			// Open a new console by pressing 'c'
			console_task = create_task(get_current_task(), (void (*)(void *))console_task_procedure, 0x00010000, TASK_PRIORITY_SHELL);
			console_task_argument = malloc(sizeof(*console_task_argument));
			console_task_argument->background_sheet = sheet;
			console_task_return = malloc(sizeof(*console_task_return));
			console_task_return->task_type = TASK_TYPE_CONSOLE;
			printf_serial("console_task->segment_selector = %#06x\n", console_task->segment_selector);
			start_task(console_task, console_task_argument, console_task_return, 1);
			break;
		case KEY_T:
			// Start test task by pressing 't'
			test_task = create_task(get_current_task(), (void (*)(void *))test_task_procedure, 0x00010000, TASK_PRIORITY_USER);
			test_task_argument = malloc(sizeof(*test_task_argument));
			test_task_argument->background_sheet = sheet;
			test_task_return = malloc(sizeof(*test_task_return));
			test_task_return->task_type = TASK_TYPE_TEST;
			printf_serial("test_task->segment_selector = %#06x\n", test_task->segment_selector);
			start_task(test_task, test_task_argument, test_task_return, 1);
			break;
		case KEY_W:
			// Open a new window by pressing 'w'
			create_window("Hello, World!", sheet, 0, 0, 0x0200, 0x0200, get_current_task()->event_queue);
			break;
		}
		return NULL;
	case EVENT_TYPE_MOUSE_EVENT:
		printf_serial("Mouse event @ background sheet.\n");
		return NULL;
	case EVENT_TYPE_TIMER_EVENT:
		printf_serial("Timer event @ background sheet.\n");
		return NULL;
	case EVENT_TYPE_SHEET_CREATED:
		printf_serial("Sheet created event @ background sheet.\n");
		// Test sheet
		translucent_red.red = 0xff;
		translucent_red.green = 0x00;
		translucent_red.blue = 0x00;
		translucent_red.alpha = 0x80;
		translucent_green.red = 0x00;
		translucent_green.green = 0xff;
		translucent_green.blue = 0x00;
		translucent_green.alpha = 0x80;
		translucent_blue.red = 0x00;
		translucent_blue.green = 0x00;
		translucent_blue.blue = 0xff;
		translucent_blue.alpha = 0x80;
		translucent_red_sheet = create_sheet(sheet, 0x0000, 0x0000, 0x0100, 0x0100, NULL, get_current_task()->event_queue);
		translucent_green_sheet = create_sheet(sheet, 0x0080, 0x0080, 0x0100, 0x0100, NULL, get_current_task()->event_queue);
		translucent_blue_sheet = create_sheet(sheet, 0x0100, 0x0100, 0x0100, 0x0100, NULL, get_current_task()->event_queue);
		fill_box_sheet(translucent_red_sheet, 0, 0, translucent_red_sheet->width, translucent_red_sheet->height, translucent_red);
		fill_box_sheet(translucent_green_sheet, 0, 0, translucent_green_sheet->width, translucent_green_sheet->height, translucent_green);
		fill_box_sheet(translucent_blue_sheet, 0, 0, translucent_blue_sheet->width, translucent_blue_sheet->height, translucent_blue);
		return NULL;
	case EVENT_TYPE_SHEET_CLICKED:
	case EVENT_TYPE_SHEET_FOCUSED:
	case EVENT_TYPE_SHEET_UNFOCUSED:
	case EVENT_TYPE_SHEET_USER_DEFINED:
	case EVENT_TYPE_SHEET_VERTICAL_WHEEL:
		return default_event_procedure(sheet, event);
	case EVENT_TYPE_TASK_DELETION_RESPONSE:
		task_return = (TaskReturn*)event->event_union.task_deletion_response_event.returns;
		switch(task_return->task_type)
		{
		case TASK_TYPE_CONSOLE:
			printf_serial("Detect console task deletion response, segment selector = %#06x.\n", event->event_union.task_deletion_response_event.segment_selector);
			break;
		case TASK_TYPE_COMMAND:
			clean_up_command_task(event->event_union.task_deletion_response_event.task, event->event_union.task_deletion_response_event.arguments);
			break;
		case TASK_TYPE_TEST:
			printf_serial("Detect test task deletion response, segment selector = %#06x.\n", event->event_union.task_deletion_response_event.segment_selector);
			break;
		default:
			ERROR(); // Invalid task type
			break;
		}
		free(event->event_union.task_deletion_response_event.returns);
		free(event->event_union.task_deletion_response_event.arguments);
		free_global_segment(event->event_union.task_deletion_response_event.segment_selector);
		return NULL;
	default:
		return NULL;
	}
}

void test_task_procedure(TestTaskArgument *test_task_argument)
{
				//{red ,green, blue,alpha}
	Color foreground_color	= {0x00, 0x00, 0x00, 0xff};
	Color background_color	= {0x80, 0x80, 0x80, 0xff};
	Queue *event_queue;
	Task *task;
	Timer *print_counter_timer;
	unsigned long long counter = 0;
	Window *window;
	printf_serial("Hello, Test Task!\n");
	task = get_current_task();
	event_queue = create_event_queue(task);
	window = create_window("Test Task", test_task_argument->background_sheet, 8 * task->segment_selector, 8 * task->segment_selector, 0x0100, 0x0100, event_queue);
	print_counter_timer = create_timer(0, 100, event_queue, NULL, NULL, NULL);
	while(true)
	{
		Event new_event;
		Event const *event = dequeue(event_queue);
		if(event)switch(event->type)
		{
		case EVENT_TYPE_TIMER_EVENT:
			if(event->event_union.timer_event.timer == print_counter_timer)
			{
				if(sheet_exists(window->client_sheet))printf_sheet(window->client_sheet, 0, 0, foreground_color, background_color, "counter = %#018llx", counter);
			}
			else call_timer_procedure(event->event_union.timer_event.timer);
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
				printf_serial("Enqueue test task deletion request!\n");
			}
			break;
		case EVENT_TYPE_TASK_DELETION_REQUEST:
			printf_serial("Detect test task deletion request.\n");
			delete_timer(print_counter_timer);
			close_task(task);
			ERROR(); // Can't close task!
			break;
		default: // invalid event->type
			ERROR();
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else counter++;
	}
}

