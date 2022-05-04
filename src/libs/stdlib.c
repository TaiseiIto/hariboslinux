#include "common.h"
#include "stdlib.h"
#include "system_call.h"

typedef struct _ComHeader
{
	void *text_base;
	void *rodata_base;
	void *data_base;
	void *bss_base;
	void *common_base;
	void *common_deletion_prevention_base;
	void *heap_and_stack_base;
	unsigned int heap_and_stack_size;
} ComHeader;

typedef struct _MemorySection
{
	struct _MemorySection *previous;
	struct _MemorySection *next;
	size_t size; // This doesn't include sizeof(MemorySection)
	unsigned char flags;
	#define MEMORY_SECTION_ALLOCATED 0x01
} MemorySection;

ComHeader *com_header = NULL;
MemorySection *root_memory_section = NULL;

void exit(int status)
{
	system_call_exit(status);
}

void free(void *address)
{
	MemorySection *memory_section;
	memory_section = (MemorySection *)(address - sizeof(*memory_section));
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
	// else // double free error!
	return;
}

void *malloc(size_t size)
{
	MemorySection *memory_section;
	if(!size)return NULL;
	if(!root_memory_section)root_memory_section = com_header->heap_and_stack_base;
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
			return allocated;
		}
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
	return NULL;
}

