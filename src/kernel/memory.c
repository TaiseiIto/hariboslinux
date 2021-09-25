#include "gdt.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "task.h"

MemorySection *root_memory_section;
void * const heap_base = (void *)0x00400000;

void free(void *address)
{
	MemorySection *memory_section;
	cli_task();
	memory_section = root_memory_section;
	do
	{
		if((void *)memory_section + sizeof(*memory_section) == address)
		{
			if(memory_section->flags & MEMORY_SECTION_ALLOCATED)
			{
				memory_section->flags &= ~MEMORY_SECTION_ALLOCATED;
				if((void *)memory_section->previous + sizeof(*memory_section->previous) + memory_section->previous->size == (void *)memory_section && memory_section->previous->flags == memory_section->flags) // merge memory_section and memory_section->previous
				{
					if(root_memory_section == memory_section)root_memory_section = memory_section->previous;
					memory_section->previous->next = memory_section->next;
					memory_section->next->previous = memory_section->previous;
					memory_section->previous->size += sizeof(*memory_section) + memory_section->size;
					memory_section = memory_section->previous;
				}
				if((void *)memory_section + sizeof(*memory_section) + memory_section->size == (void *)memory_section->next && memory_section->next->flags == memory_section->flags) // merge memory_section and memory_section->next
				{
					if(root_memory_section == memory_section->next)root_memory_section = memory_section;
					memory_section->size += sizeof(*memory_section->next) + memory_section->next->size;
					memory_section->next->next->previous = memory_section;
					memory_section->next = memory_section->next->next;
				}
			}
			else // double free error!
			{
			}
			sti_task();
			return;
		}
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
}

MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index)
{
	MemoryRegionDescriptor memory_region_descriptor;
	reads(memory_map_segment_selector, (void *)(index * sizeof(MemoryRegionDescriptor)), &memory_region_descriptor, sizeof(memory_region_descriptor));
	return memory_region_descriptor;
}

MemorySection const *get_root_memory_section(void)
{
	return root_memory_section;
}

void init_memory(void)
{
	MemoryRegionDescriptor memory_region_descriptor;
	MemorySection *memory_section;
	unsigned int memory_region_descriptor_index = 0;;
	root_memory_section = NULL;
	print_serial("Check memory regions\n");
	do
	{
		memory_region_descriptor = get_memory_region_descriptor(memory_region_descriptor_index);
		printf_serial("base = %#018llx, length = %#018llx, type = %#010x, attribute = %#010x\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		if(memory_region_descriptor.type == 0x00000001 && (unsigned long long int)(unsigned int)heap_base + sizeof(MemorySection) < memory_region_descriptor.base + memory_region_descriptor.length && memory_region_descriptor.base < 0x0000000100000000)
		{
			if(memory_region_descriptor.base < (unsigned long long int)(unsigned int)heap_base)
			{
				memory_section = heap_base;
				if(0x0000000100000000 < memory_region_descriptor.base + memory_region_descriptor.length)memory_section->size = 0x00000000 - (unsigned int)heap_base - sizeof(*memory_section);
				else memory_section->size = (unsigned int)(memory_region_descriptor.base + memory_region_descriptor.length - (unsigned long long int)(unsigned int)heap_base) - sizeof(*memory_section);
			}
			else
			{
				memory_section = (MemorySection *)(unsigned int)memory_region_descriptor.base;
				if(0x0000000100000000 < memory_region_descriptor.base + memory_region_descriptor.length)memory_section->size = 0x00000000 - (unsigned int)memory_section - sizeof(*memory_section);
				else memory_section->size = (unsigned int)(memory_region_descriptor.length) - sizeof(*memory_section);
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
	print_serial("\nCheck memory sections\n");
	memory_section = root_memory_section;
	do
	{
		printf_serial("previous = %p, this = %p, next = %p, size = %#010x, flags = %#04x\n", memory_section->previous, memory_section, memory_section->next, memory_section->size, memory_section->flags);
		memory_section = memory_section->next;
	}while(memory_section != root_memory_section);
	new_line_serial();
}

void *malloc(size_t size)
{
	MemorySection *memory_section;
	cli_task();
	memory_section = root_memory_section;
	do
	{
		if(!(memory_section->flags & MEMORY_SECTION_ALLOCATED) && size <= memory_section->size)
		{
			void *allocated = (void *)memory_section + sizeof(*memory_section);
			if(memory_section->size <= size + sizeof(MemorySection))memory_section->flags |= MEMORY_SECTION_ALLOCATED; // Can't devide the memory section
			else // devide the memory section
			{
				MemorySection *remaining_memory_section = (MemorySection *)((void *)memory_section + sizeof(*memory_section) + size);
				remaining_memory_section->previous = memory_section;
				remaining_memory_section->next = memory_section->next;
				remaining_memory_section->size = memory_section->size - size - sizeof(*remaining_memory_section);
				remaining_memory_section->flags = memory_section->flags;
				memory_section->next->previous = remaining_memory_section;
				memory_section->next = remaining_memory_section;
				memory_section->size = size;
				memory_section->flags |= MEMORY_SECTION_ALLOCATED;
			}
			root_memory_section = memory_section->next;
			sti_task();
			return allocated;
		}
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
	sti_task();
	return NULL;
}

