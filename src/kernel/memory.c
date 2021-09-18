#include "gdt.h"
#include "io.h"
#include "memory.h"
#include "serial.h"

MemoryRegionDescriptor getMemoryRegionDescriptor(unsigned int index)
{
	MemoryRegionDescriptor memoryRegionDescriptor;
	reads(memory_map_segment_selector, (void *)(index * sizeof(MemoryRegionDescriptor)), &memoryRegionDescriptor, sizeof(memoryRegionDescriptor));
	return memoryRegionDescriptor;
}

void init_memory(void)
{
	MemoryRegionDescriptor memoryRegionDescriptor;
	unsigned int memoryRegionDescriptorIndex = 0;;
	do
	{
		memoryRegionDescriptor = getMemoryRegionDescriptor(memoryRegionDescriptorIndex);
		printf_serial_polling("base = %#018llX, length = %#018llX, type = %#010X, attribute = %#010X\n", memoryRegionDescriptor.base, memoryRegionDescriptor.length, memoryRegionDescriptor.type, memoryRegionDescriptor.attribute);
		memoryRegionDescriptorIndex++;
	} while(memoryRegionDescriptor.base != 0 || memoryRegionDescriptor.length != 0 || memoryRegionDescriptor.type != 0 || memoryRegionDescriptor.attribute != 0);
}

