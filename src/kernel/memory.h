#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "stddef.h"

#define MEMORY_MAP_VIDEO_INFORMATION	((void*)0x00000600)
#define MEMORY_MAP_BOOT_INFORMATION	((void*)0x00000800)
#define MEMORY_MAP_LOADED_DISK_BEGIN	((void*)0x00100000)
#define MEMORY_MAP_BOOT_SECTOR_BEGIN	((void*)0x00100000)
#define MEMORY_MAP_BOOT_SECTOR_END	((void*)0x00100200)
#define MEMORY_MAP_FIRST_FAT_BEGIN	((void*)0x00100200)
#define MEMORY_MAP_FIRST_FAT_END	((void*)0x00101400)
#define MEMORY_MAP_SECOND_FAT_BEGIN	((void*)0x00101400)
#define MEMORY_MAP_SECOND_FAT_END	((void*)0x00102600)
#define MEMORY_MAP_ROOT_DIRECTORY_BEGIN	((void*)0x00102600)
#define MEMORY_MAP_ROOT_DIRECTORY_END	((void*)0x00104200)
#define MEMORY_MAP_LOADDISK_BIN_BEGIN	((void*)0x00104200)
#define MEMORY_MAP_LOADDISK_BIN_END	((void*)0x00104800)
#define MEMORY_MAP_GETMEMMP_BIN_BEGIN	((void*)0x00104800)
#define MEMORY_MAP_GETMEMMP_BIN_END	((void*)0x00104e00)
#define MEMORY_MAP_INITSCRN_BIN_BEGIN	((void*)0x00104e00)
#define MEMORY_MAP_INITSCRN_BIN_END	((void*)0x00105800)
#define MEMORY_MAP_MV2PRTMD_BIN_BEGIN	((void*)0x00105800)
#define MEMORY_MAP_MV2PRTMD_BIN_END	((void*)0x00105c00)
#define MEMORY_MAP_DPLYDISK_BIN_BEGIN	((void*)0x00105c00)
#define MEMORY_MAP_DPLYDISK_BIN_END	((void*)0x00106000)
#define MEMORY_MAP_KERNEL_BIN_BEGIN	((void*)0x00106000)
#define MEMORY_MAP_LOADED_DISK_END	((void*)0x00268000)
#define MEMORY_MAP_IDT_BEGIN		((void*)0x00268000)
#define MEMORY_MAP_IDT_END		((void*)0x00268800)
#define MEMORY_MAP_GDT_BEGIN		((void*)0x00270000)
#define MEMORY_MAP_GDT_END		((void*)0x00280000)
#define MEMORY_MAP_KERNEL_STACK_BEGIN	((void*)0x00280000)
#define MEMORY_MAP_KERNEL_STACK_END	((void*)0x00300000)
#define MEMORY_MAP_KERNEL_HEAP_BEGIN	((void*)0x00400000)

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

