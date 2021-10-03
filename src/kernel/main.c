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
#include "serial.h"
#include "stdio.h"
#include "task.h"
#include "timer.h"

BootInformation get_boot_information(void);

void main(void)
{
	BootInformation boot_information;
	Color background_color;
	Color color;
	Color foreground_color;
	unsigned short keyboard_flags = 0;
	MemoryRegionDescriptor memory_region_descriptor;
	MemorySection const *memory_section;
	unsigned int memory_region_descriptor_index;
	unsigned short screen_text_row = 5;
	unsigned int timer_interrupt_counter = 0;
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
	init_timer();
	print_serial("finish init_timer()\n\n");
	init_keyboard();
	print_serial("finish init_keyboard()\n\n");
	init_mouse();
	print_serial("finish init_mouse()\n\n");
	init_screen();
	print_serial("finish init_screen()\n\n");
	boot_information = get_boot_information();
	print_serial("finish get_boot_information()\n\n");
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "mouse ID = %#04x", get_mouse_id());
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "keyboard state = %#04x", boot_information.keyboard_state);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded cylinder = %#04x", boot_information.last_loaded_cylinder);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded head = %#04x", boot_information.last_loaded_head);
	printf_screen(0x0000, screen_text_row++ * CHAR_HEIGHT, foreground_color, background_color, "last loaded sector = %#04x", boot_information.last_loaded_sector);
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
	sti_task();
	print_serial("finish sti_task()\n\n");
	init_serial_interrupt();
	print_serial("finish init_serial_interrupt()\n\n");
	while(1)
	{
		Event const *event;
		event = dequeue_event();
		if(event)switch(event->type)
		{
		case EVENT_TYPE_KEYBOARD_EVENT:
			if((keyboard_flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)) != (event->event_union.keyboard_event.flags & (KEYBOARD_FLAG_LAYOUT_ENGLISH | KEYBOARD_FLAG_LAYOUT_JAPANESE)))
			{
				if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_ENGLISH)print_screen(0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = English ");
				else if(event->event_union.keyboard_event.flags & KEYBOARD_FLAG_LAYOUT_JAPANESE)print_screen(0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "keyboard layout = Japanese");
			}
			if(event->event_union.keyboard_event.character)printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "keyboard event character = %c", event->event_union.keyboard_event.character);
			keyboard_flags = event->event_union.keyboard_event.flags;
			break;
		case EVENT_TYPE_KEYBOARD_INTERRUPT:
			printf_screen(0x0000, 0x0002 * CHAR_HEIGHT, foreground_color, background_color, "keyboard interrupt signal = %#04x", event->event_union.keyboard_interrupt.signal);
			printf_serial("keyboard interrupt signal = %#04x\n", event->event_union.keyboard_interrupt.signal);
			decode_keyboard_interrupt(event->event_union.keyboard_interrupt.signal);
			break;
		case EVENT_TYPE_MOUSE_INTERRUPT:
			printf_screen(0x0000, 0x0003 * CHAR_HEIGHT, foreground_color, background_color, "mouse interrupt signal = %#04x", event->event_union.mouse_interrupt.signal);
			printf_serial("mouse interrupt signal = %#04x\n", event->event_union.mouse_interrupt.signal);
			break;
		case EVENT_TYPE_TIMER_INTERRUPT:
			if(++timer_interrupt_counter % 0x100 == 0)
			{
				printf_screen(0x0000, 0x0004 * CHAR_HEIGHT, foreground_color, background_color, "timer_interrupt_counter = %#010x", timer_interrupt_counter);
				printf_serial("timer_interrupt_counter = %#010x\n", timer_interrupt_counter);
			}
			break;
		default: // invalid event->type
			printf_serial("invalid event->type %#04x\n", event->type);
			break;
		}
		else hlt();
	}
}

BootInformation get_boot_information(void)
{
	BootInformation boot_information;
	reads(boot_information_segment_selector, (void *)0x00000000, (void *)&boot_information, sizeof(boot_information));
	return boot_information;
}

