#include "boot.h"
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

BootInformation get_boot_information(void);

void main(void)
{
	BootInformation boot_information;
	Color background_color;
	Color color;
	Color foreground_color;
	MemoryRegionDescriptor memory_region_descriptor;
	unsigned int memory_region_descriptor_index;
	cli();
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n\n");
	init_gdt();
	init_idt();
	init_memory();
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
	printf_screen(0x0000, 0 * CHAR_HEIGHT, foreground_color, background_color, "keyboard state = %#04X", boot_information.keyboard_state);
	printf_screen(0x0000, 1 * CHAR_HEIGHT, foreground_color, background_color, "last loaded cylinder = %#04X", boot_information.last_loaded_cylinder);
	printf_screen(0x0000, 2 * CHAR_HEIGHT, foreground_color, background_color, "last loaded head = %#04X", boot_information.last_loaded_head);
	printf_screen(0x0000, 3 * CHAR_HEIGHT, foreground_color, background_color, "last loaded sector = %#04X", boot_information.last_loaded_sector);
	memory_region_descriptor_index = 0;
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_screen(0x0000, (memory_region_descriptor_index + 4) * CHAR_HEIGHT, foreground_color, background_color, "memory region base = %#018llX, length = %#018llX, type = %#010X", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type);
		memory_region_descriptor_index++;
	} while(memory_region_descriptor.base != 0 || memory_region_descriptor.length != 0 || memory_region_descriptor.type != 0 || memory_region_descriptor.attribute != 0);
	new_line_serial_polling();
	sti();
	while(1)hlt();
}

BootInformation get_boot_information(void)
{
	BootInformation boot_information;
	reads(boot_information_segment_selector, (void *)0x00000000, (void *)&boot_information, sizeof(boot_information));
	return boot_information;
}

