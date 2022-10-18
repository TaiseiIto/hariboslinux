#include "gdt.h"
#include "io.h"
#include "memory.h"
#include "serial.h"
#include "stdlib.h"
#include "task.h"

BIOSDataArea const * const bios_data_area = MEMORY_MAP_BIOS_DATA_AREA;
MemorySection *root_memory_section;
void * const heap_base = MEMORY_MAP_KERNEL_HEAP_BEGIN;

void free(void *address)
{
	MemorySection *memory_section;
	cli_task();
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
	else ERROR(); // double free error!
	sti_task();
	return;
}

BIOSDataArea const *get_bios_data_area(void)
{
	return bios_data_area;
}

MemoryRegionDescriptor get_memory_region_descriptor(unsigned int index)
{
	return ((MemoryRegionDescriptor const *)MEMORY_MAP_MEMORY_INFORMATION)[index];
}

MemorySection const *get_root_memory_section(void)
{
	return root_memory_section;
}

unsigned int get_free_memory_space_size(void)
{
	unsigned int free_memory_space_size = 0;
	MemorySection *memory_section;
	cli_task();
	memory_section = root_memory_section;
	do
	{
		if(!(memory_section->flags & MEMORY_SECTION_ALLOCATED))free_memory_space_size += memory_section->size;
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
	sti_task();
	return free_memory_space_size;
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
		printf_serial_without_malloc("base = %#018llx, length = %#018llx, type = %#010x, attribute = %#010x\n", memory_region_descriptor.base, memory_region_descriptor.length, memory_region_descriptor.type, memory_region_descriptor.attribute);
		if(memory_region_descriptor.type == MEMORY_REGION_AVAILABLE && (unsigned long long int)(unsigned int)heap_base + sizeof(MemorySection) < memory_region_descriptor.base + memory_region_descriptor.length && memory_region_descriptor.base < 0x0000000100000000)
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
		printf_serial_without_malloc("previous = %p, this = %p, next = %p, size = %#010x, flags = %#04x\n", memory_section->previous, memory_section, memory_section->next, memory_section->size, memory_section->flags);
		memory_section = memory_section->next;
	}while(memory_section != root_memory_section);
	new_line_serial();
}

void *malloc(size_t size)
{
	MemorySection *memory_section;
	if(!size)
	{
		ERROR();
		return NULL;
	}
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
	ERROR(); // Can't find enough size free memory section!
	return NULL;
}

void print_bios_data_area(BIOSDataArea const *bios_data_area)
{
	for(unsigned int i = 0; i < _countof(bios_data_area->com_port_address); i++)printf_serial("bios_data_area->com_port_address[%d] = %#6.4x\n", i, bios_data_area->com_port_address[i]);
	for(unsigned int i = 0; i < _countof(bios_data_area->lpt_port_address); i++)printf_serial("bios_data_area->lpt_port_address[%d] = %#6.4x\n", i, bios_data_area->lpt_port_address[i]);
	printf_serial("bios_data_area->ebda_base_address = %#6.4x\n", bios_data_area->ebda_base_address);
	printf_serial("bios_data_area->equipment_list_flags = %#6.4x\n", bios_data_area->equipment_list_flags);
	printf_serial("bios_data_area->pcjr = %#4.2x\n", bios_data_area->pcjr);
	printf_serial("bios_data_area->memory_size_before_ebda_kilobytes = %#6.4x\n", bios_data_area->memory_size_before_ebda_kilobytes);
	printf_serial("bios_data_area->reserved = %#4.2x\n", bios_data_area->reserved);
	printf_serial("bios_data_area->ps2_bios_control_flags = %#4.2x\n", bios_data_area->ps2_bios_control_flags);
	printf_serial("bios_data_area->keyboard_flags = %#6.4x\n", bios_data_area->keyboard_flags);
	printf_serial("bios_data_area->keypad_entry = %#4.2x\n", bios_data_area->keypad_entry);
	printf_serial("bios_data_area->keyboard_buffer_head = %#6.4x\n", bios_data_area->keyboard_buffer_head);
	printf_serial("bios_data_area->keyboard_buffer_tail = %#6.4x\n", bios_data_area->keyboard_buffer_tail);
	for(unsigned i = 0; i < _countof(bios_data_area->keyboard_buffer); i++)printf_serial("bios_data_area->keyboard_buffer[%#04.2x] = %#4.2x\n", i, bios_data_area->keyboard_buffer[i]);
	printf_serial("bios_data_area->drive_recalibration_status = %#4.2x\n", bios_data_area->drive_recalibration_status);
	printf_serial("bios_data_area->diskette_motor_status = %#4.2x\n", bios_data_area->diskette_motor_status);
}

