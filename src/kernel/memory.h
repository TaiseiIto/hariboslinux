#ifndef _MEMORY_H_
#define _MEMORY_H_

typedef struct
{
	unsigned long long base;
	unsigned long long length;
	unsigned int type;
	unsigned int attribute;
} MemoryRegionDescriptor;

MemoryRegionDescriptor getMemoryRegionDescriptor(unsigned int index);
void init_memory(void);

#endif

