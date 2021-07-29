#include "gdt.h"
#include "io.h"
#include "serial.h"

#define GDT_ADDR ((void *)0x00200000)

void init_gdt(void)
{
	SegmentDescriptor null_segment;
	SegmentDescriptor whole_memory_segment;
	SegmentDescriptor kernel_code_segment;
	SegmentDescriptor kernel_data_segment;
	SegmentDescriptor gdt_segment;
	//SegmentDescriptor vram_segment;
	SegmentDescriptor segment_checker;
	void *source;
	void *destination;

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

	kernel_code_segment.limit_low = 0x2dff;
	kernel_code_segment.base_low = 0xce00;
	kernel_code_segment.base_mid = 0x00;
	kernel_code_segment.access_right = SEGMENT_DESCRIPTOR_READABLE | SEGMENT_DESCRIPTOR_EXECUTABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	kernel_code_segment.limit_high = 0x09 | SEGMENT_DESCRIPTOR_SIZE;
	kernel_code_segment.base_high = 0x00;

	kernel_data_segment.limit_low = 0x01f3;
	kernel_data_segment.base_low = 0xce00;
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

	//vram_segment.limit_low = 0xffff;
	//vram_segment.base_low = 0x0000;
	//vram_segment.base_mid = 0x0a;
	//vram_segment.access_right = SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	//vram_segment.limit_high = 0x01 | SEGMENT_DESCRIPTOR_SIZE;
	//vram_segment.base_high = 0x00;

	// init new GDT
	for(destination = GDT_ADDR; destination < GDT_ADDR + 0x2000 * sizeof(SegmentDescriptor); destination += sizeof(SegmentDescriptor))writes(&null_segment, WHOLE_SEGMENT, destination, sizeof(null_segment));

	// set new GDT
	destination = GDT_ADDR;
	writes(&null_segment, WHOLE_SEGMENT, destination, sizeof(null_segment));
	destination += sizeof(null_segment);
	writes(&whole_memory_segment, WHOLE_SEGMENT, destination, sizeof(whole_memory_segment));
	destination += sizeof(whole_memory_segment);
	writes(&kernel_code_segment, WHOLE_SEGMENT, destination, sizeof(kernel_code_segment));
	destination += sizeof(kernel_code_segment);
	writes(&kernel_data_segment, WHOLE_SEGMENT, destination, sizeof(kernel_data_segment));
	destination += sizeof(kernel_data_segment);
	writes(&gdt_segment, WHOLE_SEGMENT, destination, sizeof(gdt_segment));
	destination += sizeof(gdt_segment);
	//writes(&vram_segment, WHOLE_SEGMENT, destination, sizeof(vram_segment));
	//destination += sizeof(vram_segment);
	
	// load new GDT
	lgdt(0xffff, (SegmentDescriptor *)GDT_ADDR);

	// VRAM
	set_segment(0x000a0000, 0x0001ffff, SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA, 0x00);

	// check new GDT
	print_serial_polling("check new GDT\n");
	source = (void *)0x00000000 + sizeof(null_segment);
	do
	{
		reads(GDT_SEGMENT, source, &segment_checker, sizeof(segment_checker));
		printf_serial_polling("Segment Descriptor %#010X\n", source);
		printf_serial_polling("\tlimit low\t%#06X\n", segment_checker.limit_low);
		printf_serial_polling("\tbase low\t%#06X\n", segment_checker.base_low);
		printf_serial_polling("\tbase mid\t%#04X\n", segment_checker.base_mid);
		printf_serial_polling("\taccess right\t%#04X\n", segment_checker.access_right);
		printf_serial_polling("\tlimit high\t%#04X\n", segment_checker.limit_high);
		printf_serial_polling("\tbase high\t%#04X\n", segment_checker.base_high);
		source += sizeof(segment_checker);
	}while(segment_checker.access_right & SEGMENT_DESCRIPTOR_PRESENT);
	new_line_serial_polling();
}

// return a new segment selector
// return 0 if failed
unsigned short set_segment(unsigned int base, unsigned int limit, unsigned char access_right, unsigned char flags)
{
	for(void *segment_selector = (void *)0x00000008; segment_selector != 0x00000000; segment_selector += sizeof(SegmentDescriptor))
	{
		SegmentDescriptor segment_descriptor;
		reads(GDT_SEGMENT, segment_selector, &segment_descriptor, sizeof(segment_descriptor));
		if(!(segment_descriptor.access_right & SEGMENT_DESCRIPTOR_PRESENT))
		{
			segment_descriptor.base_low = (unsigned short)(base & 0x0000ffff);
			segment_descriptor.base_mid = (unsigned char)(base >> 16 & 0x000000ff);
			segment_descriptor.base_high = (unsigned char)(base >> 24 & 0x000000ff);
			segment_descriptor.access_right = access_right | SEGMENT_DESCRIPTOR_PRESENT;
			segment_descriptor.limit_high = flags | SEGMENT_DESCRIPTOR_SIZE;
			if(0x00100000 <= limit)
			{
				segment_descriptor.limit_high |= SEGMENT_DESCRIPTOR_GRANULARITY;
				limit >>= 10;
			}
			segment_descriptor.limit_low = (unsigned short)(limit & 0x0000ffff);
			segment_descriptor.limit_high |= (unsigned char)(limit >> 16 & 0x0000000f);
			writes(&segment_descriptor, GDT_SEGMENT, (void *)segment_selector, sizeof(segment_descriptor));
			return (unsigned int)segment_selector;
		}
	}
	// unused segment not found
	return 0x0000;
}

