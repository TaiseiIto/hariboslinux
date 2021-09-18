#ifndef _MEMORY_H_
#define _MEMORY_H_

#define NULL ((void *)0x00000000)

typedef struct _MemorySection
{
	struct _MemorySection *previous;
	struct _MemorySection *next;
	unsigned int size; // This doesn't include sizeof(MemorySection)
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

MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index);
MemorySection const *get_root_memory_section(void);
void init_memory(void);

#endif

