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

BootInformation get_boot_information(void);

void main(void)
{
	BootInformation boot_information;
	Color background_color;
	Color color;
	Color foreground_color;
	MemoryRegionDescriptor memory_region_descriptor;
	MemorySection const *memory_section;
	unsigned int memory_region_descriptor_index;
	unsigned short screen_text_row = 2;
	cli();
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n\n");
	init_gdt();
	init_idt();
	init_task();
	init_memory();
	create_event_queue();
	init_pic();
	init_keyboard();
	init_mouse();
	init_screen();
	boot_information = get_boot_information();
	background_color.red = 0x00;
	background_color.green = 0x00;
	background_color.blue = 0x00;
	foreground_color.red = 0xff;
	foreground_color.green = 0xff;
	foreground_color.blue = 0xff;
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
	new_line_serial_polling();
	sti_task();
	while(1)
	{
		Event const *event = dequeue_event();
		if(event)switch(event->type)
		{
		case EVENT_TYPE_KEYBOARD_INTERRUPT:
			printf_serial_polling("keyboard interrupt signal = %#04x\n", event->event_union.keyboard_interrupt_event.signal);
			printf_screen(0x0000, 0x0000 * CHAR_HEIGHT, foreground_color, background_color, "keyboard interrupt signal = %#04x\n", event->event_union.keyboard_interrupt_event.signal);
			break;
		case EVENT_TYPE_MOUSE_INTERRUPT:
			printf_serial_polling("mouse interrupt signal = %#04x\n", event->event_union.mouse_interrupt_event.signal);
			printf_screen(0x0000, 0x0001 * CHAR_HEIGHT, foreground_color, background_color, "mouse interrupt signal = %#04x\n", event->event_union.mouse_interrupt_event.signal);
			break;
		default: // invalid event->type
			printf_serial_polling("invalid event->type\n");
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

