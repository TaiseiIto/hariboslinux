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
#include "stdio.h"
#include "task.h"
#include "timer.h"

void main(void)
{
	BootInformation const * const boot_information = (BootInformation const * const)0x00000800;
	Color background_color;
	Color color;
	Color foreground_color;
	unsigned short keyboard_flags = 0;
	MemoryRegionDescriptor memory_region_descriptor;
	MemorySection const *memory_section;
	unsigned int memory_region_descriptor_index;
	unsigned short screen_text_row = 8;
	Timer *test_timer;
	Timer *checking_free_memory_space_size_timer;
	unsigned long long timer_counter = 0;
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
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "mouse ID = %#04x", get_mouse_id());
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "keyboard state = %#04x", boot_information->keyboard_state);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded cylinder = %#04x", boot_information->last_loaded_cylinder);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded head = %#04x", boot_information->last_loaded_head);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded sector = %#04x", boot_information->last_loaded_sector);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "memory regions");
	memory_region_descriptor_index = 0;
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "base = %#018llx, length = %#018llx, type = %#010x, attribute = %#010x\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		memory_region_descriptor_index++;
	} while(memory_region_descriptor.base != 0 || memory_region_descriptor.length != 0 || memory_region_descriptor.type != 0 || memory_region_descriptor.attribute != 0);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "memory sections");
	memory_section = get_root_memory_section();
	do
	{
		printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "previous = %p, this = %p, next = %p, size = %#010x, flags = %#04x\n", memory_section->previous, memory_section, memory_section->next, memory_section->size, memory_section->flags);
		memory_section = memory_section->next;
	} while(memory_section != get_root_memory_section());
	test_timer = create_timer(0, 100);
	checking_free_memory_space_size_timer = create_timer(0, 100);
	init_serial_interrupt();
	sti_task();
	print_serial("finish sti_task()\n\n");
	while(1)
	{
		Event const *event;
		event = dequeue_event();
		if(event)switch(event->type)
		{
		case EVENT_TYPE_KEYBOARD_EVENT:
			if((keyboard_flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)) != (event->event_union.keyboard_event.flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)))
			{
				if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_ENGLISH)print_screen(0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = English ");
				else if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_JAPANESE)print_screen(0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = Japanese");
			}
			if(event->event_union.keyboard_event.character)printf_screen(0x0000, 0x0002 * CHAR_HEIGHT, foreground_color, background_color, "keyboard event character = %c", event->event_union.keyboard_event.character);
			keyboard_flags = event->event_union.keyboard_event.flags;
			break;
		case EVENT_TYPE_KEYBOARD_INTERRUPT:
			printf_screen(0x0000, 0x0003 * CHAR_HEIGHT, foreground_color, background_color, "keyboard interrupt signal = %#04x", event->event_union.keyboard_interrupt.signal);
			printf_serial("keyboard interrupt signal = %#04x\n", event->event_union.keyboard_interrupt.signal);
			decode_keyboard_interrupt(event->event_union.keyboard_interrupt.signal);
			break;
		case EVENT_TYPE_MOUSE_EVENT:
			printf_screen(0x0000, 0x0004 * CHAR_HEIGHT, foreground_color, background_color, "mouse state x=%4d, y=%4d, vertical wheel=%4d, horizontal wheel=%4d, left button=%c, middle button=%c, right button=%c, 4th button=%c, 5th button=%c", event->event_union.mouse_event.x_movement, event->event_union.mouse_event.y_movement, event->event_union.mouse_event.vertical_wheel_movement, event->event_union.mouse_event.horizontal_wheel_movement, (event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED) ? '1' : '0');
			printf_serial("mouse state x=%4d, y=%4d, vertical wheel=%4d, horizontal wheel=%4d, left button=%c, middle button=%c, right button=%c, 4th button=%c, 5th button=%c\n", event->event_union.mouse_event.x_movement, event->event_union.mouse_event.y_movement, event->event_union.mouse_event.vertical_wheel_movement, event->event_union.mouse_event.horizontal_wheel_movement, (event->event_union.mouse_event.flags & MOUSE_LEFT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_MIDDLE_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_RIGHT_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_4TH_BUTTON_PUSHED) ? '1' : '0', (event->event_union.mouse_event.flags & MOUSE_5TH_BUTTON_PUSHED) ? '1' : '0');
			break;
		case EVENT_TYPE_MOUSE_INTERRUPT:
			printf_screen(0x0000, 0x0005 * CHAR_HEIGHT, foreground_color, background_color, "mouse interrupt signal = %#04x", event->event_union.mouse_interrupt.signal);
			printf_serial("mouse interrupt signal = %#04x\n", event->event_union.mouse_interrupt.signal);
			decode_mouse_interrupt(event->event_union.mouse_interrupt.signal);
			break;
		case EVENT_TYPE_PIT_INTERRUPT:
			decode_pit_interrupt();
			break;
		case EVENT_TYPE_RTC_INTERRUPT:
			printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "%04d/%02d/%02d %s %02d:%02d:%02d", event->event_union.rtc_interrupt.year, event->event_union.rtc_interrupt.month, event->event_union.rtc_interrupt.day, get_day_of_week_string(get_day_of_week(event->event_union.rtc_interrupt.year, event->event_union.rtc_interrupt.month, event->event_union.rtc_interrupt.day)), event->event_union.rtc_interrupt.hour, event->event_union.rtc_interrupt.minute, event->event_union.rtc_interrupt.second);
			printf_serial("second = %d\n", event->event_union.rtc_interrupt.second);
			printf_serial("minute = %d\n", event->event_union.rtc_interrupt.minute);
			printf_serial("hour = %d\n", event->event_union.rtc_interrupt.hour);
			printf_serial("day = %d\n", event->event_union.rtc_interrupt.day);
			printf_serial("month = %d\n", event->event_union.rtc_interrupt.month);
			printf_serial("year = %d\n", event->event_union.rtc_interrupt.year);
			break;
		case EVENT_TYPE_TIMER_EVENT:
			if(event->event_union.timer_event.timer == test_timer)
			{
				timer_counter++;
				printf_screen(0x0000, 0x0006 * CHAR_HEIGHT, foreground_color, background_color, "timer = %lld seconds", timer_counter);
				printf_serial("timer_counterr = %lld seconds\n", timer_counter);
			}
			else if(event->event_union.timer_event.timer == checking_free_memory_space_size_timer)
			{
				printf_screen(0x0000, 0x0007 * CHAR_HEIGHT, foreground_color, background_color, "free memory space size = %u bytes", get_free_memory_space_size());
				printf_serial("free memory space size = %u bytes\n", get_free_memory_space_size());
			}
			break;
		default: // invalid event->type
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else hlt();
	}
}

