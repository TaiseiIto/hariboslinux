#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "stddef.h"

typedef struct _MemorySection
{
	struct _MemorySection *previous;
	struct _MemorySection *next;
	size_t size; // This doesn't include sizeof(MemorySection)
	unsigned char flags;
	#define MEMORY_SECTION_ALLOCATED 0x01
} MemorySection;

typedef struct
{
	unsigned long long base;
	unsigned long long length;
	unsigned int type;
	unsigned int attribute;
} MemoryRegionDescriptor;

void free(void *address);
MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index);
MemorySection const *get_root_memory_section(void);
unsigned int get_free_memory_space_size(void);
void init_memory(void);
void *malloc(size_t size);

#endif

