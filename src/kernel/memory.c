#include "gdt.h"
#include "io.h"
#include "memory.h"
#include "serial.h"

typedef struct _MemorySection
{
	struct _MemorySection *previous;
	struct _MemorySection *next;
	unsigned int size; // This doesn't include sizeof(MemorySection)
	unsigned char flags;
	#define MEMORY_SECTION_ALLOCATED 0x01
} MemorySection;

MemorySection *root_memory_section;
void * const heap_base = (void *)0x00400000;

MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index)
{
	MemoryRegionDescriptor memory_region_descriptor;
	reads(memory_map_segment_selector, (void *)(index * sizeof(MemoryRegionDescriptor)), &memory_region_descriptor, sizeof(memory_region_descriptor));
	return memory_region_descriptor;
}

void init_memory(void)
{
	MemoryRegionDescriptor memory_region_descriptor;
	MemorySection *memory_section;
	unsigned int memory_region_descriptor_index = 0;;
	root_memory_section = NULL;
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_serial_polling("base = %#018llX, length = %#018llX, type = %#010X, attribute = %#010X\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		if(memory_region_descriptor.type == 0x00000001 && (unsigned long long int)(unsigned int)heap_base + sizeof(MemorySection) < memory_region_descriptor.base + memory_region_descriptor.length && memory_region_descriptor.base < 0x0000000100000000)
		{
			if(memory_region_descriptor.base < (unsigned long long int)(unsigned int)heap_base)
			{
				memory_section = heap_base;
				if(0x0000000100000000 < memory_region_descriptor.base + memory_region_descriptor.length)memory_section->size = 0x00000000 - (unsigned int)heap_base;
				else memory_section->size = (unsigned int)(memory_region_descriptor.base + memory_region_descriptor.length - (unsigned long long int)(unsigned int)heap_base);
			}
			else
			{
				memory_section = (MemorySection *)(unsigned int)memory_region_descriptor.base;
				if(0x0000000100000000 < memory_region_descriptor.base + memory_region_descriptor.length)memory_section->size = 0x00000000 - (unsigned int)memory_section;
				else memory_section->size = (unsigned int)(memory_region_descriptor.length);
			}
			memory_section->flags = 0x00;
			if(!root_memory_section)
			{
				root_memory_section = memory_section;
				root_memory_section->previous = root_memory_section;
				root_memory_section->next = root_memory_section;
			}
			else
			{
				memory_section->previous = root_memory_section->previous;
				memory_section->next = root_memory_section;
				root_memory_section->previous->next = memory_section;
				root_memory_section->previous = memory_section;
			}
		}
		memory_region_descriptor_index++;
	} while(memory_region_descriptor.base != 0 || memory_region_descriptor.length != 0 || memory_region_descriptor.type != 0 || memory_region_descriptor.attribute != 0);
}

