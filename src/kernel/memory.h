#ifndef _MEMORY_H_
#define _MEMORY_H_

typedef struct
{
	unsigned long base;
	unsigned long length;
	unsigned int type;
	unsigned int attribute;
} MemoryRegionDescriptor;

MemoryRegionDescriptor getMemoryRegionDescriptor(unsigned int index);

#endif

