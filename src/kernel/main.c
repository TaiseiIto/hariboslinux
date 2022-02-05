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

void main(void)
{
	BootInformation const * const boot_information = (BootInformation const * const)0x00000800;
	Color background_color;
	Color foreground_color;
	Color opaque_red;
	Color opaque_green;
	Color opaque_blue;
	Color translucent_red;
	Color translucent_green;
	Color translucent_blue;
	MemoryRegionDescriptor memory_region_descriptor;
	Sheet *background_sheet;
	Sheet *mouse_cursor_sheet;
	Sheet *opaque_red_sheet;
	Sheet *opaque_green_sheet;
	Sheet *opaque_blue_sheet;
	Sheet *translucent_red_sheet;
	Sheet *translucent_green_sheet;
	Sheet *translucent_blue_sheet;
	Timer *test_timer;
	Timer *checking_free_memory_space_size_timer;
	unsigned int memory_region_descriptor_index;
	unsigned long long timer_counter = 0;
	unsigned short keyboard_flags = 0;
	unsigned short screen_text_row = 8;
	Window *test_window;
	cli();
	new_line_serial();
	print_serial("Hello, kernel.bin!\n\n");
	init_gdt();
	print_serial("finish init_gdt()\n\n");
	init_idt();
	print_serial("finish init_idt()\n\n");
	init_task();
	print_serial("finish init_task()\n\n");
	init_memory();
	print_serial("finish init_memory()\n\n");
	create_event_queue();
	print_serial("finish create_event_queue()\n\n");
	init_pic();
	print_serial("finish init_pic()\n\n");
	init_pit();
	print_serial("finish init_pit()\n\n");
	init_keyboard();
	print_serial("finish init_keyboard()\n\n");
	init_rtc();
	print_serial("finish init_rtc()\n\n");
	init_mouse();
	print_serial("finish init_mouse()\n\n");
	init_screen();
	print_serial("finish init_screen()\n\n");
	init_serial_interrupt();
	sti_task();
	print_serial("finish sti_task()\n\n");
	init_sheets(&background_sheet, &mouse_cursor_sheet);
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	background_color.alpha = 0xff;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	foreground_color.alpha = 0xff;
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
	opaque_red_sheet = create_sheet(background_sheet, 0x0000, 0x0000, 0x0100, 0x0100, NULL);
	opaque_green_sheet = create_sheet(background_sheet, 0x0100, 0x0100, 0x0100, 0x0100, NULL);
	opaque_blue_sheet = create_sheet(background_sheet, 0x0200, 0x0200, 0x0100, 0x0100, NULL);
	translucent_red_sheet = create_sheet(background_sheet, 0x0080, 0x0080, 0x0100, 0x0100, NULL);
	translucent_green_sheet = create_sheet(background_sheet, 0x0180, 0x0180, 0x0100, 0x0100, NULL);
	translucent_blue_sheet = create_sheet(background_sheet, 0x0280, 0x0280, 0x0100, 0x0100, NULL);
	fill_box_sheet(opaque_red_sheet, 0, 0, opaque_red_sheet->width, opaque_red_sheet->height, opaque_red);
	fill_box_sheet(opaque_green_sheet, 0, 0, opaque_green_sheet->width, opaque_green_sheet->height, opaque_green);
	fill_box_sheet(opaque_blue_sheet, 0, 0, opaque_blue_sheet->width, opaque_blue_sheet->height, opaque_blue);
	fill_box_sheet(translucent_red_sheet, 0, 0, translucent_red_sheet->width, translucent_red_sheet->height, translucent_red);
	fill_box_sheet(translucent_green_sheet, 0, 0, translucent_green_sheet->width, translucent_green_sheet->height, translucent_green);
	fill_box_sheet(translucent_blue_sheet, 0, 0, translucent_blue_sheet->width, translucent_blue_sheet->height, translucent_blue);
	test_window = create_window(background_sheet, 0, 0, 0x0200, 0x0200);
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
	test_timer = create_timer(0, 100);
	checking_free_memory_space_size_timer = create_timer(0, 100);
	while(1)
	{
		Event new_event;
		Event const *event;
		Sheet *operated_sheet;
		event = dequeue_event();
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
			operated_sheet = get_uppest_sheet(background_sheet, event->event_union.mouse_event.x, event->event_union.mouse_event.y);
			if(event->event_union.mouse_event.flags & (MOUSE_LEFT_BUTTON_PUSHED_NOW | MOUSE_LEFT_BUTTON_RELEASED_NOW | MOUSE_MIDDLE_BUTTON_PUSHED_NOW | MOUSE_MIDDLE_BUTTON_RELEASED_NOW | MOUSE_RIGHT_BUTTON_PUSHED_NOW | MOUSE_RIGHT_BUTTON_RELEASED_NOW | MOUSE_4TH_BUTTON_PUSHED_NOW | MOUSE_4TH_BUTTON_RELEASED_NOW | MOUSE_5TH_BUTTON_PUSHED_NOW | MOUSE_5TH_BUTTON_RELEASED_NOW))
			{
				new_event.type = EVENT_TYPE_SHEET_CLICKED;
				new_event.event_union.sheet_clicked_event.sheet = operated_sheet;
				new_event.event_union.sheet_clicked_event.x = event->event_union.mouse_event.x - get_sheet_x_on_screen(operated_sheet);
				new_event.event_union.sheet_clicked_event.y = event->event_union.mouse_event.y - get_sheet_y_on_screen(operated_sheet);
				if(event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_MIDDLE_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_RIGHT_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_4TH_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_PUSHED | SHEET_CLICKED_EVENT_FLAG_5TH_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_RELEASED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_LEFT_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_RELEASED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_MIDDLE_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_RELEASED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_RIGHT_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_RELEASED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_4TH_BUTTON;
				else if(event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_RELEASED_NOW)new_event.event_union.sheet_clicked_event.flags = SHEET_CLICKED_EVENT_FLAG_RELEASED | SHEET_CLICKED_EVENT_FLAG_5TH_BUTTON;
				enqueue_event(&new_event);
			}
			if(event->event_union.mouse_event.x_movement || event->event_union.mouse_event.y_movement)
			{
				new_event.type = EVENT_TYPE_SHEET_MOUSE_MOVE;
				new_event.event_union.sheet_mouse_move_event.sheet = operated_sheet;
				new_event.event_union.sheet_mouse_move_event.x_movement = event->event_union.mouse_event.x_movement;
				new_event.event_union.sheet_mouse_move_event.y_movement = event->event_union.mouse_event.y_movement;
				enqueue_event(&new_event);
				move_sheet(mouse_cursor_sheet, event->event_union.mouse_event.x, event->event_union.mouse_event.y);
			}
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
			event->event_union.sheet_clicked_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_CREATED:
			event->event_union.sheet_created_event.sheet->event_procedure(event->event_union.sheet_created_event.sheet, event);
			break;
		case EVENT_TYPE_SHEET_MOUSE_MOVE:
			event->event_union.sheet_mouse_move_event.sheet->event_procedure(event->event_union.sheet_clicked_event.sheet, event);
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
		default: // invalid event->type
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else hlt();
	}
}

