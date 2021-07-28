#include "gdt.h"
#include "io.h"
#include "segment.h"

#define GDT_ADDR ((void *)0x00200000)

void init_gdt(void)
{
	SegmentDescriptor null_segment;
	SegmentDescriptor whole_memory_segment;
	SegmentDescriptor kernel_code_segment;
	SegmentDescriptor kernel_data_segment;
	SegmentDescriptor vram_segment;
	void *destination = GDT_ADDR;

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

	vram_segment.limit_low = 0xffff;
	vram_segment.base_low = 0x0000;
	vram_segment.base_mid = 0x0a;
	vram_segment.access_right = SEGMENT_DESCRIPTOR_WRITABLE | SEGMENT_DESCRIPTOR_CODE_OR_DATA | SEGMENT_DESCRIPTOR_PRESENT;
	vram_segment.limit_high = 0x01 | SEGMENT_DESCRIPTOR_SIZE;
	vram_segment.base_high = 0x00;

	writes(&null_segment, WHOLE_SEGMENT, destination, sizeof(null_segment));
	destination += sizeof(null_segment);
	writes(&whole_memory_segment, WHOLE_SEGMENT, destination, sizeof(whole_memory_segment));
	destination += sizeof(whole_memory_segment);
	writes(&kernel_code_segment, WHOLE_SEGMENT, destination, sizeof(kernel_code_segment));
	destination += sizeof(kernel_code_segment);
	writes(&kernel_data_segment, WHOLE_SEGMENT, destination, sizeof(kernel_data_segment));
	destination += sizeof(kernel_data_segment);
	writes(&vram_segment, WHOLE_SEGMENT, destination, sizeof(vram_segment));
	destination += sizeof(vram_segment);
}

