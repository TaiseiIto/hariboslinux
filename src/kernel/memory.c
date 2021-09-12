#include "gdt.h"
#include "io.h"
#include "memory.h"

MemoryRegionDescriptor getMemoryRegionDescriptor(unsigned int index)
{
	MemoryRegionDescriptor memoryRegionDescriptor;
	reads(memory_map_segment_selector, (void *)(index * sizeof(MemoryRegionDescriptor)), &memoryRegionDescriptor, sizeof(memoryRegionDescriptor));
	return memoryRegionDescriptor;
}

