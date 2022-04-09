#include "disk.h"
#include "memory.h"
#include "pic.h"
#include "serial.h"

BootSector const * const boot_sector = (BootSector const * const)MEMORY_MAP_LOADED_DISK_BEGIN;

unsigned int cluster_size;
void **file_allocation_tables;
void *first_sector;
FileInformation *root_directory;

void disk_interrupt_handler(void)
{
	finish_interruption(IRQ_DISK);
	print_serial("disk interrupt\n");
}

void init_file_system(void)
{
	cluster_size = boot_sector->number_of_sectors_per_cluster * boot_sector->sector_size;
	first_sector = (void *)boot_sector + boot_sector->first_sector_number * boot_sector->sector_size;
	file_allocation_tables = malloc(boot_sector->number_of_file_allocation_tables * sizeof(*file_allocation_tables));
	for(unsigned int i = 0; i < boot_sector->number_of_file_allocation_tables; i++)file_allocation_tables[i] = first_sector + i * boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size;
	root_directory = (FileInformation *)(file_allocation_tables[boot_sector->number_of_file_allocation_tables - 1] + boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size);
	printf_serial("cluster_size = %#010.8x\n", cluster_size);
	printf_serial("first_sector = %p\n", first_sector);
	for(unsigned int i = 0; i < boot_sector->number_of_file_allocation_tables; i++)printf_serial("file_allocation_tables[%#04.2x] = %p", i, file_allocation_tables[i]);
}

void primary_ATA_hard_disk_interrupt_handler(void)
{
	finish_interruption(IRQ_PRIMARY_ATA_HARD_DISK);
	print_serial("primary ATA hard disk interrupt\n");
}

void secondary_ATA_hard_disk_interrupt_handler(void)
{
	finish_interruption(IRQ_SECONDARY_ATA_HARD_DISK);
	print_serial("secondary ATA hard disk interrupt\n");
}

