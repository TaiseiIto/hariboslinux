#include "chain_string.h"
#include "disk.h"
#include "memory.h"
#include "pic.h"
#include "serial.h"
#include "stdlib.h"

BootSector const * const boot_sector = (BootSector const * const)MEMORY_MAP_LOADED_DISK_BEGIN;

unsigned int cluster_size;
void **file_allocation_tables;
void *first_sector;
FileInformation *root_directory_entries;

char *create_file_name(FileInformation const *file_information)
{
	unsigned char name_length;
	unsigned char extension_length;
	for(name_length = 0; name_length < _countof(file_information->name); name_length++)if(file_information->name[name_length] == ' ')break;
	for(extension_length = 0; extension_length < _countof(file_information->extension); extension_length++)if(file_information->extension[extension_length] == ' ')break;
	printf_serial("name_length = %d\n", name_length);
	printf_serial("extension_length = %d\n", extension_length);
	return create_format_char_array("%*s.%*s", name_length, file_information->name, extension_length, file_information->extension);
}

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
	root_directory_entries = (FileInformation *)(file_allocation_tables[boot_sector->number_of_file_allocation_tables - 1] + boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size);
	printf_serial("Jump instruction = %#04x %#04x %#04x\n", boot_sector->jump_instruction[0], boot_sector->jump_instruction[1], boot_sector->jump_instruction[2]);
	printf_serial("Product name = \"%.8s\"\n", boot_sector->product_name);
	printf_serial("Sector size = %#06.4x\n", boot_sector->sector_size);
	printf_serial("Number of sectors per cluster = %#04.2x\n", boot_sector->number_of_sectors_per_cluster);
	printf_serial("First sector number = %#06.4x\n", boot_sector->first_sector_number);
	printf_serial("Number of file allocation tables = %#04.2x\n", boot_sector->number_of_file_allocation_tables);
	printf_serial("Number of root directory entries = %#06.4x\n", boot_sector->number_of_root_directory_entries);
	printf_serial("Number of sectors = %#06.4x\n", boot_sector->number_of_sectors);
	printf_serial("Media type = %#04.2x\n", boot_sector->media_type);
	printf_serial("Number of sectors per file allocation table = %#06.4x\n", boot_sector->number_of_sectors_per_file_allocation_table);
	printf_serial("Number of sectors per track = %#06.4x\n", boot_sector->number_of_sectors_per_track);
	printf_serial("Number of heads = %#06.4x\n", boot_sector->number_of_heads);
	printf_serial("Number of hidden sectors = %#010.8x\n", boot_sector->number_of_hidden_sectors);
	printf_serial("Number of sectors double word = %#010.8x\n", boot_sector->number_of_sectors_double_word);
	printf_serial("Drive number = %#04.2x\n", boot_sector->drive_number);
	printf_serial("Boot signature = %#04.2x\n", boot_sector->boot_signature);
	printf_serial("Volume serial number = %#010.8x\n", boot_sector->volume_serial_number);
	printf_serial("Volume label = \"%.11s\"\n", boot_sector->volume_label);
	printf_serial("File system name = \"%.8s\"\n", boot_sector->file_system_name);

	printf_serial("cluster_size = %#010.8x\n", cluster_size);
	printf_serial("first_sector = %p\n", first_sector);
	for(unsigned int i = 0; i < boot_sector->number_of_file_allocation_tables; i++)printf_serial("file_allocation_tables[%#04.2x] = %p\n", i, file_allocation_tables[i]);
	printf_serial("root_directory_entries = %p\n", root_directory_entries);
	for(FileInformation const *file_information = root_directory_entries; file_information->name[0]; file_information++)
	{
		char *file_name = create_file_name(file_information);
		printf_serial("%s\n", file_name);
		free(file_name);
	}
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

