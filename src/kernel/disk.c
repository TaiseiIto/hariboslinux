#include "chain_string.h"
#include "disk.h"
#include "memory.h"
#include "pic.h"
#include "serial.h"
#include "stdlib.h"
#include "string.h"

BootSector const * const boot_sector = (BootSector const * const)MEMORY_MAP_LOADED_DISK_BEGIN;

unsigned int cluster_size;
unsigned int number_of_clusters;
void const *cluster0;
void const **file_allocation_tables;
void const *first_sector;
FileInformation const *root_directory_entries;

char *create_file_name(FileInformation const *file_information)
{
	unsigned char name_length;
	unsigned char extension_length;
	for(name_length = 0; name_length < _countof(file_information->name); name_length++)if(file_information->name[name_length] == ' ')break;
	for(extension_length = 0; extension_length < _countof(file_information->extension); extension_length++)if(file_information->extension[extension_length] == ' ')break;
	return create_format_char_array("%.*s.%.*s", name_length, file_information->name, extension_length, file_information->extension);
}

void disk_interrupt_handler(void)
{
	finish_interruption(IRQ_DISK);
	print_serial("disk interrupt\n");
}

void const *get_cluster(unsigned short cluster_number)
{
	return cluster0 + cluster_number * cluster_size;
}

FileInformation const *get_file_information(char *file_name)
{
	bool found = false;
	for(unsigned int i = 0; i < boot_sector->number_of_root_directory_entries; i++) // Search file informations.
	{
		FileInformation const *candidate;
		candidate = root_directory_entries + i;
		if(candidate->name[0])
		{
			char *candidate_name = create_file_name(candidate);
			if(!strcmp(file_name, candidate_name))found = true;
			free(candidate_name);
			if(found)return candidate;
		}
		else break;
	}
	return NULL; // The file is not found.
}

unsigned int get_file_updated_year(FileInformation const *file_information)
{
	return 1980 + ((file_information->date & FILE_INFORMATION_DATE_MASK_YEAR) >> 9);
}

unsigned char get_file_updated_month(FileInformation const *file_information)
{
	return (file_information->date & FILE_INFORMATION_DATE_MASK_MONTH) >> 5;
}

unsigned char get_file_updated_day(FileInformation const *file_information)
{
	return file_information->date & FILE_INFORMATION_DATE_MASK_DAY;
}

unsigned char get_file_updated_hour(FileInformation const *file_information)
{
	return (file_information->time & FILE_INFORMATION_TIME_MASK_HOUR) >> 11;
}

unsigned char get_file_updated_minute(FileInformation const *file_information)
{
	return (file_information->time & FILE_INFORMATION_TIME_MASK_MINUTE) >> 5;
}

unsigned char get_file_updated_second(FileInformation const *file_information)
{
	return 2 * file_information->time & FILE_INFORMATION_TIME_MASK_BISECOND;
}

unsigned short get_next_cluster_number(unsigned short cluster_number)
{
	unsigned short next_cluster_number = 0;
	if(cluster_number % 2)
	{
		next_cluster_number = ((unsigned char const **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 2];
		next_cluster_number <<= 4;
		next_cluster_number += ((unsigned char const **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 1] >> 4 & 0x0f;
	}
	else
	{
		next_cluster_number = ((unsigned char const **)file_allocation_tables)[0][cluster_number / 2 * 3 + 1] & 0x0f;
		next_cluster_number <<= 8;
		next_cluster_number += ((unsigned char const **)file_allocation_tables)[0][cluster_number / 2 * 3];
	}
	return next_cluster_number;
}

void init_file_system(void)
{

	cluster_size = boot_sector->number_of_sectors_per_cluster * boot_sector->sector_size;
	number_of_clusters = (boot_sector->number_of_sectors + boot_sector->number_of_sectors_per_cluster - 1) / boot_sector->number_of_sectors_per_cluster;
	first_sector = (void *)boot_sector + boot_sector->first_sector_number * boot_sector->sector_size;
	file_allocation_tables = malloc(boot_sector->number_of_file_allocation_tables * sizeof(*file_allocation_tables));
	for(unsigned int i = 0; i < boot_sector->number_of_file_allocation_tables; i++)file_allocation_tables[i] = first_sector + i * boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size;
	root_directory_entries = (FileInformation *)(file_allocation_tables[boot_sector->number_of_file_allocation_tables - 1] + boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size);
	cluster0 = (void const *)((((unsigned int)(root_directory_entries + boot_sector->number_of_root_directory_entries) + cluster_size - 1) / cluster_size - 2) * cluster_size);
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
	for(unsigned int i = 0; i < boot_sector->number_of_root_directory_entries; i++)
	{
		FileInformation const *file_information;
		file_information = root_directory_entries + i;
		if(file_information->name[0])
		{
			char *file_name;
			file_name = create_file_name(file_information);
			printf_serial("%s\n", file_name);
			free(file_name);
			printf_serial("\tupdated time %04d/%02d/%02d %02d:%02d:%02d\n", get_file_updated_year(file_information), get_file_updated_month(file_information), get_file_updated_day(file_information), get_file_updated_hour(file_information), get_file_updated_minute(file_information), get_file_updated_second(file_information));
			printf_serial("\tsize %#010.8x bytes\n", file_information->size);
			printf_serial("\tcluster number %#06.4x\n", file_information->cluster_number);
		}
	}
}

void *load_file(char *file_name)
{
	FileInformation const * file_information = get_file_information(file_name);
	if(file_information)
	{
		void *loaded_address = malloc(file_information->size);
		void *writer = loaded_address;
		unsigned int unwritten_size = file_information->size;
		for(unsigned short cluster_number = file_information->cluster_number; cluster_number <= number_of_clusters + 1; cluster_number = get_next_cluster_number(cluster_number))
		{
			unsigned int copied_size = cluster_size <= unwritten_size ? cluster_size : unwritten_size;
			memcpy(writer, get_cluster(cluster_number), copied_size);
			writer += copied_size;
			unwritten_size -= copied_size;
		}
		return loaded_address;
	}
	else return NULL; // The file is not found.
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

