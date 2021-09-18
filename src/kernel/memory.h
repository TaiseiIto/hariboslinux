#ifndef _MEMORY_H_
#define _MEMORY_H_

#define NULL ((void *)0x00000000)

typedef struct
{
	unsigned long long base;
	unsigned long long length;
	unsigned int type;
	unsigned int attribute;
} MemoryRegionDescriptor;

MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index);
void init_memory(void);

#endif

