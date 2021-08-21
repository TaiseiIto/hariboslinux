#include "boot.h"
#include "gdt.h"
#include "io.h"
#include "serial.h"

#define GDT_ADDR ((SegmentDescriptor *)0x00200000)

unsigned short whole_memory_segment_selector;
unsigned short kernel_code_segment_selector;
unsigned short kernel_data_segment_selector;
unsigned short gdt_segment_selector;
unsigned short idt_segment_selector;
unsigned short boot_information_segment_selector;
unsigned short loaded_disk_segment_selector;
unsigned short first_fat_segment_selector;
unsigned short second_fat_segment_selector;
unsigned short root_directory_entry_segment_selector;
unsigned short vram_segment_selector;

void init_gdt(void)
{
	SegmentDescriptor null_segment;
	SegmentDescriptor whole_memory_segment;
	SegmentDescriptor kernel_code_segment;
	SegmentDescriptor kernel_data_segment;
	SegmentDescriptor gdt_segment;
	SegmentDescriptor segment_checker;
	SegmentDescriptor *source;
	SegmentDescriptor *destination;
	unsigned short const temporary_whole_memory_segment_selector = 0x0008;

	null_segment.limit_low = 0x0000;
	null_segment.base_low = 0x0000;
	null_segment.base_mid = 0x00;
	null_segment.access_right = 0x00;
	null_segment.limit_high = 0x00;
	null_segment.base_high = 0x00;

	whole_memory_segment.limit_low = 0xffff;
	whole_memory_segment.base_low = 0x0000;
	whole_memory_segment.base_mid = 0x00;
	whole_memory_segment.access_right = SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	whole_memory_segment.limit_high = 0x0f | SEGMENT_DESCRIPTOR_SIZE | SEGMENT_DESCRIPTOR_GRANULARITY;
	whole_memory_segment.base_high = 0x00;

	kernel_code_segment.limit_low = 0x29ff;
	kernel_code_segment.base_low = 0xd200;
	kernel_code_segment.base_mid = 0x00;
	kernel_code_segment.access_right = SEGMENT_DESCRIPTOR_READABLE | SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	kernel_code_segment.limit_high = 0x09 | SEGMENT_DESCRIPTOR_SIZE;
	kernel_code_segment.base_high = 0x00;

	kernel_data_segment.limit_low = 0x01f2;
	kernel_data_segment.base_low = 0xd200;
	kernel_data_segment.base_mid = 0x00;
	kernel_data_segment.access_right = SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	kernel_data_segment.limit_high = 0x00 | SEGMENT_DESCRIPTOR_SIZE | SEGMENT_DESCRIPTOR_GRANULARITY;
	kernel_data_segment.base_high = 0x00;

	gdt_segment.limit_low = 0xffff;
	gdt_segment.base_low = 0x0000;
	gdt_segment.base_mid = 0x20;
	gdt_segment.access_right = SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	gdt_segment.limit_high = 0x00 | SEGMENT_DESCRIPTOR_SIZE;
	gdt_segment.base_high = 0x00;

	// init new GDT
	for(destination = GDT_ADDR; destination != GDT_ADDR + 0x2000; destination ++)writes(&null_segment, temporary_whole_memory_segment_selector, destination, sizeof(null_segment));

	// set new GDT
	destination = GDT_ADDR;
	writes(&null_segment, temporary_whole_memory_segment_selector, destination++, sizeof(null_segment));
	writes(&whole_memory_segment, temporary_whole_memory_segment_selector, destination, sizeof(whole_memory_segment));
	whole_memory_segment_selector = (unsigned int)destination++;
	writes(&kernel_code_segment, temporary_whole_memory_segment_selector, destination, sizeof(kernel_code_segment));
	kernel_code_segment_selector = (unsigned int)destination++;
	writes(&kernel_data_segment, temporary_whole_memory_segment_selector, destination, sizeof(kernel_data_segment));
	kernel_data_segment_selector = (unsigned int)destination++;
	writes(&gdt_segment, temporary_whole_memory_segment_selector, destination, sizeof(gdt_segment));
	gdt_segment_selector = (unsigned int)destination++;
	
	// load new GDT
	lgdt(0xffff, GDT_ADDR);

	idt_segment_selector = set_segment(0x00007400, 0x000007ff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	boot_information_segment_selector = set_segment(0x00000500, sizeof(BootInformation), SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	loaded_disk_segment_selector = set_segment(0x00007c00, 0x00097fff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	first_fat_segment_selector = set_segment(0x00007e00, 0x000011ff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	second_fat_segment_selector = set_segment(0x00009000, 0x000011ff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	root_directory_entry_segment_selector = set_segment(0x0000a200, 0x00001bff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	vram_segment_selector = set_segment(0x000a0000, 0x0001ffff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA);

	// check new GDT
	print_serial_polling("check new GDT\n");
	printf_serial_polling("whole_memory_segment_selector = %#06X\n", whole_memory_segment_selector);
	printf_serial_polling("kernel_code_segment_selector = %#06X\n", kernel_code_segment_selector);
	printf_serial_polling("kernel_data_segment_selector = %#06X\n", kernel_data_segment_selector);
	printf_serial_polling("gdt_segment_selector = %#06X\n", gdt_segment_selector);
	printf_serial_polling("idt_segment_selector = %#06X\n", idt_segment_selector);
	printf_serial_polling("boot_information_segment_selector = %#06X\n", boot_information_segment_selector);
	printf_serial_polling("loaded_disk_segment_selector = %#06X\n", loaded_disk_segment_selector);
	printf_serial_polling("first_fat_segment_selector = %#06X\n", first_fat_segment_selector);
	printf_serial_polling("second_fat_segment_selector = %#06X\n", second_fat_segment_selector);
	printf_serial_polling("root_directory_entry_segment_selector = %#06X\n", root_directory_entry_segment_selector);
	printf_serial_polling("vram_segment_selector = %#06X\n", vram_segment_selector);
	new_line_serial_polling();
	source = (SegmentDescriptor *)0x00000008;
	do
	{
		reads(gdt_segment_selector, source, &segment_checker, sizeof(segment_checker));
		printf_serial_polling("Segment Descriptor %#010X\n", source);
		printf_serial_polling("\tlimit_low\t%#06X\n", segment_checker.limit_low);
		printf_serial_polling("\tbase_low\t%#06X\n", segment_checker.base_low);
		printf_serial_polling("\tbase_mid\t%#04X\n", segment_checker.base_mid);
		printf_serial_polling("\taccess_right\t%#04X\n", segment_checker.access_right);
		printf_serial_polling("\tlimit_high\t%#04X\n", segment_checker.limit_high);
		printf_serial_polling("\tbase_high\t%#04X\n", segment_checker.base_high);
		source++;
	}while(segment_checker.access_right & SEGMENT_DESCRIPTOR_PRESENT);
	new_line_serial_polling();
}

// return a new segment selector
// return 0 if failed
unsigned short set_segment(unsigned int base, unsigned int limit, unsigned char access_right)
{
	for(SegmentDescriptor *segment_selector = (SegmentDescriptor *)0x00000008; segment_selector != (SegmentDescriptor*)0x00010000; segment_selector++)
	{
		SegmentDescriptor segment_descriptor;
		reads(gdt_segment_selector, segment_selector, &segment_descriptor, sizeof(segment_descriptor));
		if(!(segment_descriptor.access_right & SEGMENT_DESCRIPTOR_PRESENT))
		{
			segment_descriptor.base_low = (unsigned short)(base & 0x0000ffff);
			segment_descriptor.base_mid = (unsigned char)(base >> 16 & 0x000000ff);
			segment_descriptor.base_high = (unsigned char)(base >> 24 & 0x000000ff);
			segment_descriptor.access_right = access_right | SEGMENT_DESCRIPTOR_PRESENT;
			segment_descriptor.limit_high = SEGMENT_DESCRIPTOR_SIZE;
			if(0x00100000 <= limit)
			{
				segment_descriptor.limit_high |= SEGMENT_DESCRIPTOR_GRANULARITY;
				limit >>= 12;
			}
			segment_descriptor.limit_low = (unsigned short)(limit & 0x0000ffff);
			segment_descriptor.limit_high |= (unsigned char)(limit >> 16 & 0x0000000f);
			writes(&segment_descriptor, gdt_segment_selector, (void *)segment_selector, sizeof(segment_descriptor));
			return (unsigned int)segment_selector;
		}
	}
	// unused segment not found
	return 0x0000;
}

