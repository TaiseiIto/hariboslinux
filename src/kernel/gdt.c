#include "boot.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "serial.h"
#include "task.h"

#define GDT_ADDR ((SegmentDescriptor *)0x00270000)
#define GDT_BEGIN (GDT_ADDR + 0x00000001)
#define GDT_END (GDT_ADDR + 0x00002000)

unsigned short whole_memory_segment_selector;
unsigned short kernel_code_segment_selector;
unsigned short boot_information_segment_selector;
unsigned short memory_map_segment_selector;
unsigned short loaded_disk_segment_selector;
unsigned short first_fat_segment_selector;
unsigned short second_fat_segment_selector;
unsigned short root_directory_entry_segment_selector;

void init_gdt(void)
{
	SegmentDescriptor null_segment;
	SegmentDescriptor whole_memory_segment;
	SegmentDescriptor kernel_code_segment;
	SegmentDescriptor *segment_checker;
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

	kernel_code_segment.limit_low = 0xffff;
	kernel_code_segment.base_low = 0x0000;
	kernel_code_segment.base_mid = 0x00;
	kernel_code_segment.access_right = SEGMENT_DESCRIPTOR_READABLE | SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	kernel_code_segment.limit_high = 0x0f | SEGMENT_DESCRIPTOR_SIZE | SEGMENT_DESCRIPTOR_GRANULARITY;
	kernel_code_segment.base_high = 0x00;

	// init new GDT
	for(destination = GDT_ADDR; destination != GDT_END; destination ++)writes(&null_segment, temporary_whole_memory_segment_selector, destination, sizeof(null_segment));

	// set new GDT
	destination = GDT_ADDR;
	writes(&null_segment, temporary_whole_memory_segment_selector, destination++, sizeof(null_segment));
	writes(&whole_memory_segment, temporary_whole_memory_segment_selector, destination++, sizeof(whole_memory_segment));
	whole_memory_segment_selector = 0x0008;
	writes(&kernel_code_segment, temporary_whole_memory_segment_selector, destination++, sizeof(kernel_code_segment));
	kernel_code_segment_selector = 0x0010;
	
	// load new GDT
	lgdt(0xffff, GDT_ADDR);

	boot_information_segment_selector = set_segment(0x00000800, sizeof(BootInformation), SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	memory_map_segment_selector = set_segment(0x00000900, 0x000072ff, SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	loaded_disk_segment_selector = set_segment(0x00100000, 0x00097fff, SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	first_fat_segment_selector = set_segment(0x00100200, 0x000011ff, SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	second_fat_segment_selector = set_segment(0x00101400, 0x000011ff, SEGMENT_DESCRIPTOR_CODE_OR_DATA);
	root_directory_entry_segment_selector = set_segment(0x00102600, 0x00001bff, SEGMENT_DESCRIPTOR_CODE_OR_DATA);

	// check new GDT
	print_serial("check new GDT\n");
	printf_serial("whole_memory_segment_selector = %#06x\n", whole_memory_segment_selector);
	printf_serial("kernel_code_segment_selector = %#06x\n", kernel_code_segment_selector);
	printf_serial("boot_information_segment_selector = %#06x\n", boot_information_segment_selector);
	printf_serial("memory_map_segment_selector = %#06x\n", memory_map_segment_selector);
	printf_serial("loaded_disk_segment_selector = %#06x\n", loaded_disk_segment_selector);
	printf_serial("first_fat_segment_selector = %#06x\n", first_fat_segment_selector);
	printf_serial("second_fat_segment_selector = %#06x\n", second_fat_segment_selector);
	printf_serial("root_directory_entry_segment_selector = %#06x\n", root_directory_entry_segment_selector);
	new_line_serial();
	segment_checker = GDT_BEGIN;
	do
	{
		printf_serial("Segment Descriptor %p\n", segment_checker);
		printf_serial("\tlimit_low\t%#06x\n", segment_checker->limit_low);
		printf_serial("\tbase_low\t%#06x\n", segment_checker->base_low);
		printf_serial("\tbase_mid\t%#04x\n", segment_checker->base_mid);
		printf_serial("\taccess_right\t%#04x\n", segment_checker->access_right);
		printf_serial("\tlimit_high\t%#04x\n", segment_checker->limit_high);
		printf_serial("\tbase_high\t%#04x\n", segment_checker->base_high);
		segment_checker++;
	}while(segment_checker->access_right & SEGMENT_DESCRIPTOR_PRESENT);
	new_line_serial();
}

// return a new segment selector
// return 0 if failed
unsigned short set_segment(unsigned int base, unsigned int limit, unsigned char access_right)
{
	cli_task();
	for(SegmentDescriptor *segment_descriptor = GDT_BEGIN; segment_descriptor != GDT_END; segment_descriptor++)
	{
		if(!(segment_descriptor->access_right & SEGMENT_DESCRIPTOR_PRESENT))
		{
			segment_descriptor->base_low = (unsigned short)(base & 0x0000ffff);
			segment_descriptor->base_mid = (unsigned char)(base >> 16 & 0x000000ff);
			segment_descriptor->base_high = (unsigned char)(base >> 24 & 0x000000ff);
			segment_descriptor->access_right = access_right | SEGMENT_DESCRIPTOR_PRESENT;
			segment_descriptor->limit_high = SEGMENT_DESCRIPTOR_SIZE;
			if(0x00100000 <= limit)
			{
				segment_descriptor->limit_high |= SEGMENT_DESCRIPTOR_GRANULARITY;
				limit >>= 12;
			}
			segment_descriptor->limit_low = (unsigned short)(limit & 0x0000ffff);
			segment_descriptor->limit_high |= (unsigned char)(limit >> 16 & 0x0000000f);
			sti_task();
			return (unsigned int)segment_descriptor;
		}
	}
	sti_task();
	// unused segment not found
	ERROR_MESSAGE();
	return 0x0000;
}

