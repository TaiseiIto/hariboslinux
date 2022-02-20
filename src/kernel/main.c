#include "boot.h"
#include "event.h"
#include "font.h"
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
#include "task.h"
#include "timer.h"
#include "window.h"

typedef struct _TestTaskArgument
{
	Sheet *background_sheet;
	Task *test_task;
} TestTaskArgument;

void test_task_procedure(void *args);

void main(void)
{
	cli();
	BootInformation const * const boot_information = (BootInformation const * const)MEMORY_MAP_BOOT_INFORMATION;
	Color background_color;
	Color foreground_color;
	Color opaque_red;
	Color opaque_green;
	Color opaque_blue;
	Color translucent_red;
	Color translucent_green;
	Color translucent_blue;
	MemoryRegionDescriptor memory_region_descriptor;
	Queue *event_queue;
	Sheet *background_sheet;
	Sheet *mouse_cursor_sheet;
	Sheet *opaque_red_sheet;
	Sheet *opaque_green_sheet;
	Sheet *opaque_blue_sheet;
	Sheet *translucent_red_sheet;
	Sheet *translucent_green_sheet;
	Sheet *translucent_blue_sheet;
	Task *main_task;
	Task *test_task;
	TestTaskArgument test_task_argument;
	Timer *checking_free_memory_space_size_timer;
	Timer *test_timer;
	unsigned int memory_region_descriptor_index;
	unsigned long long timer_counter = 0;
	unsigned short keyboard_flags = 0;
	unsigned short screen_text_row = 8;
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
	event_queue = create_queue(sizeof(Event), main_task);
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
	init_serial_interrupt(main_task);
	sti_task();
	print_serial("finish init_serial_interrupt() and sti_task()\n\n");
	init_sheets(&background_sheet, &mouse_cursor_sheet, event_queue);
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
	memory_region_descriptor_index = 0;
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_sheet(background_sheet, 0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "base = %#018llx, length = %#018llx, type = %#010x, attribute = %#010x\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		memory_region_descriptor_index++;
	} while(memory_region_descriptor.base != 0 || memory_region_descriptor.length != 0 || memory_region_descriptor.type != 0 || memory_region_descriptor.attribute != 0);
	checking_free_memory_space_size_timer = create_timer(0, 100, event_queue);
	// Test timer
	test_timer = create_timer(0, 100, event_queue);
	// Test sheet
	opaque_red.red = 0xff;
	opaque_red.green = 0x00;
	opaque_red.blue = 0x00;
	opaque_red.alpha = 0xff;
	opaque_green.red = 0x00;
	opaque_green.green = 0xff;
	opaque_green.blue = 0x00;
	opaque_green.alpha = 0xff;
	opaque_blue.red = 0x00;
	opaque_blue.green = 0x00;
	opaque_blue.blue = 0xff;
	opaque_blue.alpha = 0xff;
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
	opaque_red_sheet = create_sheet(background_sheet, 0x0000, 0x0000, 0x0100, 0x0100, NULL, event_queue);
	opaque_green_sheet = create_sheet(background_sheet, 0x0100, 0x0100, 0x0100, 0x0100, NULL, event_queue);
	opaque_blue_sheet = create_sheet(background_sheet, 0x0200, 0x0200, 0x0100, 0x0100, NULL, event_queue);
	translucent_red_sheet = create_sheet(background_sheet, 0x0080, 0x0080, 0x0100, 0x0100, NULL, event_queue);
	translucent_green_sheet = create_sheet(background_sheet, 0x0180, 0x0180, 0x0100, 0x0100, NULL, event_queue);
	translucent_blue_sheet = create_sheet(background_sheet, 0x0280, 0x0280, 0x0100, 0x0100, NULL, event_queue);
	fill_box_sheet(opaque_red_sheet, 0, 0, opaque_red_sheet->width, opaque_red_sheet->height, opaque_red);
	fill_box_sheet(opaque_green_sheet, 0, 0, opaque_green_sheet->width, opaque_green_sheet->height, opaque_green);
	fill_box_sheet(opaque_blue_sheet, 0, 0, opaque_blue_sheet->width, opaque_blue_sheet->height, opaque_blue);
	fill_box_sheet(translucent_red_sheet, 0, 0, translucent_red_sheet->width, translucent_red_sheet->height, translucent_red);
	fill_box_sheet(translucent_green_sheet, 0, 0, translucent_green_sheet->width, translucent_green_sheet->height, translucent_green);
	fill_box_sheet(translucent_blue_sheet, 0, 0, translucent_blue_sheet->width, translucent_blue_sheet->height, translucent_blue);
	// Test window
	create_window("Hello, World!", background_sheet, 0, 0, 0x0200, 0x0200, event_queue);
	// Test task
	test_task = create_task(test_task_procedure, 0x00010000);
	test_task_argument.background_sheet = background_sheet;
	test_task_argument.test_task = test_task;
	start_task(test_task, &test_task_argument);
	while(1)
	{
		Event new_event;
		Event const *event;
		event = dequeue(event_queue);
		if(event)switch(event->type)
		{
		case EVENT_TYPE_CLOSE_BUTTON_CLICKED:
			if(sheet_exists(event->event_union.close_button_clicked_event.window->root_sheet))event->event_union.close_button_clicked_event.window->root_sheet->event_procedure(event->event_union.close_button_clicked_event.window->root_sheet, event);
			break;
		case EVENT_TYPE_KEYBOARD_EVENT:
			if((keyboard_flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)) != (event->event_union.keyboard_event.flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)))
			{
				if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_ENGLISH)print_sheet(background_sheet, 0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = English ");
				else if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_JAPANESE)print_sheet(background_sheet, 0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = Japanese");
			}
			if(event->event_union.keyboard_event.character)printf_sheet(background_sheet, 0x0000, 0x0002 * CHAR_HEIGHT, foreground_color, background_color, "keyboard event character = %c", event->event_union.keyboard_event.character);
			keyboard_flags = event->event_union.keyboard_event.flags;
			switch(event->event_union.keyboard_event.keycode)
			{
			case KEY_W:
				// Open a new window by pressing 'w'
				create_window("Hello, World!", background_sheet, 0, 0, 0x0200, 0x0200, event_queue);
				break;
			}
			break;
		case EVENT_TYPE_KEYBOARD_INTERRUPT:
			printf_sheet(background_sheet, 0x0000, 0x0003 * CHAR_HEIGHT, foreground_color, background_color, "keyboard interrupt signal = %#04x", event->event_union.keyboard_interrupt.signal);
			#ifdef LOGGING
			printf_serial("keyboard interrupt signal = %#04x\n", event->event_union.keyboard_interrupt.signal);
			#endif
			decode_keyboard_interrupt(event->event_union.keyboard_interrupt.signal);
			break;
		case EVENT_TYPE_MOUSE_EVENT:
			printf_sheet(background_sheet, 0x0000, 0x0004 * CHAR_HEIGHT, foreground_color, background_color, "mouse state x=%4d, y=%4d, vertical wheel=%4d, horizontal wheel=%4d, left button=%c, middle button=%c, right button=%c, 4th button=%c, 5th button=%c", event->event_union.mouse_event.x, event->event_union.mouse_event.y, event->event_union.mouse_event.vertical_wheel_movement, event->event_union.mouse_event.horizontal_wheel_movement, (event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED) ? '1' : '0');
			#ifdef LOGGING
			printf_serial("mouse state x=%4d, y=%4d, vertical wheel=%4d, horizontal wheel=%4d, left button=%c, middle button=%c, right button=%c, 4th button=%c, 5th button=%c\n", event->event_union.mouse_event.x, event->event_union.mouse_event.y, event->event_union.mouse_event.vertical_wheel_movement, event->event_union.mouse_event.horizontal_wheel_movement, (event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED) ? '1' : '0');
			#endif
			send_sheets_event(event);
			break;
		case EVENT_TYPE_MOUSE_INTERRUPT:
			printf_sheet(background_sheet, 0x0000, 0x0005 * CHAR_HEIGHT, foreground_color, background_color, "mouse interrupt signal = %#04x", event->event_union.mouse_interrupt.signal);
			#ifdef LOGGING
			printf_serial("mouse interrupt signal = %#04x\n", event->event_union.mouse_interrupt.signal);
			#endif
			decode_mouse_interrupt(event->event_union.mouse_interrupt.signal);
			break;
		case EVENT_TYPE_PIT_INTERRUPT:
			decode_pit_interrupt();
			break;
		case EVENT_TYPE_RTC_INTERRUPT:
			printf_sheet(background_sheet, 0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "%04d/%02d/%02d %s %02d:%02d:%02d", event->event_union.rtc_interrupt.year, event->event_union.rtc_interrupt.month, event->event_union.rtc_interrupt.day, get_day_of_week_string(get_day_of_week(event->event_union.rtc_interrupt.year, event->event_union.rtc_interrupt.month, event->event_union.rtc_interrupt.day)), event->event_union.rtc_interrupt.hour, event->event_union.rtc_interrupt.minute, event->event_union.rtc_interrupt.second);
			#ifdef LOGGING
			printf_serial("second = %d\n", event->event_union.rtc_interrupt.second);
			printf_serial("minute = %d\n", event->event_union.rtc_interrupt.minute);
			printf_serial("hour = %d\n", event->event_union.rtc_interrupt.hour);
			printf_serial("day = %d\n", event->event_union.rtc_interrupt.day);
			printf_serial("month = %d\n", event->event_union.rtc_interrupt.month);
			printf_serial("year = %d\n", event->event_union.rtc_interrupt.year);
			#endif
			break;
		case EVENT_TYPE_SHEET_CLICKED:
			if(sheet_exists(event->event_union.sheet_clicked_event.sheet))event->event_union.sheet_clicked_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_CREATED:
			if(sheet_exists(event->event_union.sheet_created_event.sheet))event->event_union.sheet_created_event.sheet->event_procedure(event->event_union.sheet_created_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_DELETION_REQUEST:
			if(sheet_exists(event->event_union.sheet_deletion_request_event.sheet))event->event_union.sheet_deletion_request_event.sheet->event_procedure(event->event_union.sheet_deletion_request_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_DELETION_RESPONSE:
			if(sheet_exists(event->event_union.sheet_deletion_response_event.parent))event->event_union.sheet_deletion_response_event.parent->event_procedure(event->event_union.sheet_deletion_response_event.parent, event);
			break;
		case EVENT_TYPE_SHEET_MOUSE_MOVE:
			if(sheet_exists(event->event_union.sheet_mouse_move_event.sheet))event->event_union.sheet_mouse_move_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
			break;
		case EVENT_TYPE_TIMER_EVENT:
			if(event->event_union.timer_event.timer == test_timer)
			{
				timer_counter++;
				printf_sheet(background_sheet, 0x0000, 0x0006 * CHAR_HEIGHT, foreground_color, background_color, "timer = %lld seconds", timer_counter);
				#ifdef LOGGING
				printf_serial("timer counter = %lld seconds\n", timer_counter);
				#endif
			}
			else if(event->event_union.timer_event.timer == checking_free_memory_space_size_timer)
			{
				printf_sheet(background_sheet, 0x0000, 0x0007 * CHAR_HEIGHT, foreground_color, background_color, "free memory space size = %u bytes", get_free_memory_space_size());
				#ifdef LOGGING
				printf_serial("free memory space size = %u bytes\n", get_free_memory_space_size());
				#endif
			}
			break;
		case EVENT_TYPE_WINDOW_DELETION_REQUEST:
			if(sheet_exists(event->event_union.window_deletion_request_event.window->root_sheet))event->event_union.window_deletion_request_event.window->root_sheet->event_procedure(event->event_union.window_deletion_request_event.window->root_sheet, event);
			break;
		case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			printf_serial("Window %p deleted @ main task\n", event->event_union.window_deletion_response_event.window);
			break;
		default: // invalid event->type
			ERROR_MESSAGE();
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else sleep_task(main_task);
	}
}

void test_task_procedure(void *args)
{
	Color foreground_color = {0x00, 0x00, 0x00, 0xff};
	Color background_color = {0x80, 0x80, 0x80, 0xff};
	Queue *event_queue;
	TestTaskArgument *test_task_argument;
	Timer *print_counter_timer;
	unsigned long long counter = 0;
	Window *window;
	test_task_argument = (TestTaskArgument*)args;
	event_queue = create_queue(sizeof(Event), test_task_argument->test_task);
	window = create_window("Test Task", test_task_argument->background_sheet, test_task_argument->test_task->segment_selector, test_task_argument->test_task->segment_selector, 0x0100, 0x0100, event_queue);
	print_counter_timer = create_timer(0, 100, event_queue);
	while(true)
	{
		Event const *event = dequeue(event_queue);
		if(event)switch(event->type)
		{
		case EVENT_TYPE_CLOSE_BUTTON_CLICKED:
			// Closing the window is prohibited because closing task is not implemented yet.
			break;
			if(sheet_exists(event->event_union.close_button_clicked_event.window->root_sheet))event->event_union.close_button_clicked_event.window->root_sheet->event_procedure(event->event_union.close_button_clicked_event.window->root_sheet, event);
			break;
		case EVENT_TYPE_SHEET_CLICKED:
			if(sheet_exists(event->event_union.sheet_clicked_event.sheet))event->event_union.sheet_clicked_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_CREATED:
			if(sheet_exists(event->event_union.sheet_created_event.sheet))event->event_union.sheet_created_event.sheet->event_procedure(event->event_union.sheet_created_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_DELETION_REQUEST:
			if(sheet_exists(event->event_union.sheet_deletion_request_event.sheet))event->event_union.sheet_deletion_request_event.sheet->event_procedure(event->event_union.sheet_deletion_request_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_DELETION_RESPONSE:
			if(sheet_exists(event->event_union.sheet_deletion_response_event.parent))event->event_union.sheet_deletion_response_event.parent->event_procedure(event->event_union.sheet_deletion_response_event.parent, event);
			break;
		case EVENT_TYPE_SHEET_MOUSE_MOVE:
			if(sheet_exists(event->event_union.sheet_mouse_move_event.sheet))event->event_union.sheet_mouse_move_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
			break;
		case EVENT_TYPE_TIMER_EVENT:
			if(event->event_union.timer_event.timer == print_counter_timer)
			{
				printf_sheet(window->client_sheet, 0, 0, foreground_color, background_color, "counter = %#018llx", counter);
			}
			break;
		case EVENT_TYPE_WINDOW_DELETION_REQUEST:
			if(sheet_exists(event->event_union.window_deletion_request_event.window->root_sheet))event->event_union.window_deletion_request_event.window->root_sheet->event_procedure(event->event_union.window_deletion_request_event.window->root_sheet, event);
			break;
		case EVENT_TYPE_WINDOW_DELETION_RESPONSE:
			printf_serial("Window %p deleted @ test task\n", event->event_union.window_deletion_response_event.window);
			break;
		default: // invalid event->type
			ERROR_MESSAGE();
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else counter++;
	}
}

