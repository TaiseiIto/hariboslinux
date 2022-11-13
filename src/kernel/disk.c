#include "acpi.h"
#include "bios.h"
#include "chain_string.h"
#include "common.h"
#include "disk.h"
#include "memory.h"
#include "pic.h"
#include "rtc.h"
#include "serial.h"
#include "stdlib.h"
#include "string.h"

BootSector const * const boot_sector = (BootSector const * const)MEMORY_MAP_LOADED_DISK_BEGIN;

char const * const console_file_name = "console.dev";
char const * const cpu_file_name = "cpu.dev";
char const * const disk_file_name = "disk.dev";
char const * const dsdt_file_name = "dsdt.aml";
char const * const memory_file_name = "memory.dev";
char const * const root_directory_name = "";
char const * const timer_file_name = "timer.dev";
char const * const window_file_name = "window.dev";
unsigned short const no_more_clusters = 0x0fff;

unsigned int cluster_size;
unsigned int number_of_clusters;
void *cluster0;
void **file_allocation_tables;
void *first_sector;
FileInformation *root_directory_entries;

unsigned char *sector_flags;
#define SECTOR_FLAG_CHANGED 0x01

unsigned int address2sector_number(void const *address)
{
	unsigned int disk_address = (unsigned int)address - (unsigned int)MEMORY_MAP_LOADED_DISK_BEGIN;
	unsigned int sector_number = disk_address / boot_sector->sector_size;
	return sector_number;
}

SectorSpecifier address2sector_specifier(void const *address)
{
	return sector_number2sector_specifier(address2sector_number(address));
}

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

void delete_file(char const *file_name)
{
	FileInformation *file_information = get_file_information(file_name);
	if(!file_information)ERROR(); // The fine is not found.
	if(file_information->flags & FILE_INFORMATION_FLAG_READ_ONLY_FILE)ERROR(); // The file is read only.
	// Free the file information.
	file_information->name[0] = '\0';
	sector_flags[address2sector_number(file_information)] |= SECTOR_FLAG_CHANGED;
	// Free the clusters.
	free_cluster(file_information->cluster_number);
}

void free_cluster(unsigned short cluster_number)
{
	unsigned short next_cluster_number = get_next_cluster_number(cluster_number);
	if(next_cluster_number != no_more_clusters)free_cluster(next_cluster_number);
	if(cluster_number % 2)
	{
		((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 2] = 0x00;
		((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 1] &= 0x0f;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 2])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 1])] |= SECTOR_FLAG_CHANGED;
	}
	else
	{
		((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3 + 1] &= 0xf0;
		((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3] = 0x00;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3 + 1])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3])] |= SECTOR_FLAG_CHANGED;
	}
}

void *get_cluster(unsigned short cluster_number)
{
	return cluster0 + cluster_number * cluster_size;
}

FileInformation *get_file_information(char const *file_name)
{
	bool found = false;
	for(unsigned int i = 0; i < boot_sector->number_of_root_directory_entries; i++) // Search file informations.
	{
		FileInformation *candidate;
		candidate = root_directory_entries + i;
		if(candidate->name[0])
		{
			char *candidate_name = create_file_name(candidate);
			if(!strcmp(file_name, candidate_name))found = true;
			free(candidate_name);
			if(found)return candidate;
		}
	}
	return NULL; // The file is not found.
}

unsigned int get_file_size(char const *file_name)
{
	FileInformation const *file_information = get_file_information(file_name);
	if(file_information)return file_information->size; // Return file size.
	else if(!strcmp(file_name, dsdt_file_name))return get_dsdt_aml().length; // Return size of DSDT AML.
	else if(!strcmp(file_name, root_directory_name))return boot_sector->number_of_root_directory_entries * sizeof(FileInformation); // Return size of root directory entries.
	else return 0; // File is not found.
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

unsigned short get_unused_cluster_number(void)
{
	for(unsigned short cluster_number = 2; cluster_number <= number_of_clusters + 1; cluster_number++)if(!get_next_cluster_number(cluster_number))return cluster_number;
	ERROR(); // There is no unused cluster.
	return 0;
}

FileInformation *get_unused_file_information(void)
{
	for(unsigned int i = 0; i < boot_sector->number_of_root_directory_entries; i++) // Search file informations.
	{
		FileInformation *candidate = root_directory_entries + i;
		if(!candidate->name[0])return candidate;
	}
	return NULL; // All file informations are used.
}

void init_file_system(void)
{

	cluster_size = boot_sector->number_of_sectors_per_cluster * boot_sector->sector_size;
	number_of_clusters = (boot_sector->sector_size * (boot_sector->number_of_sectors - boot_sector->first_sector_number - boot_sector->number_of_file_allocation_tables * boot_sector->number_of_sectors_per_file_allocation_table) - boot_sector->number_of_root_directory_entries * sizeof(FileInformation)) / cluster_size;
	first_sector = (void *)boot_sector + boot_sector->first_sector_number * boot_sector->sector_size;
	file_allocation_tables = malloc(boot_sector->number_of_file_allocation_tables * sizeof(*file_allocation_tables));
	for(unsigned int i = 0; i < boot_sector->number_of_file_allocation_tables; i++)file_allocation_tables[i] = first_sector + i * boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size;
	root_directory_entries = (FileInformation *)(file_allocation_tables[boot_sector->number_of_file_allocation_tables - 1] + boot_sector->number_of_sectors_per_file_allocation_table * boot_sector->sector_size);
	cluster0 = (void *)((((unsigned int)(root_directory_entries + boot_sector->number_of_root_directory_entries) + cluster_size - 1) / cluster_size - 2) * cluster_size);
	sector_flags = malloc(boot_sector->number_of_sectors);
	for(unsigned int i = 0; i < boot_sector->number_of_sectors; i++)sector_flags[i] = 0;
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
	printf_serial("number_of_clusters = %#06.4x\n", number_of_clusters);
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
	else if(!strcmp(file_name, dsdt_file_name)) // Load DSDT AML.
	{
		AMLSubstring dsdt_aml = get_dsdt_aml();
		FileInformation *loaded_address = malloc(dsdt_aml.length);
		memcpy(loaded_address, dsdt_aml.initial, dsdt_aml.length);
		return loaded_address;
	}
	else if(!strcmp(file_name, root_directory_name)) // Load root directory entries.
	{
		FileInformation *loaded_address = malloc(boot_sector->number_of_root_directory_entries * sizeof(FileInformation));
		memcpy(loaded_address, root_directory_entries, boot_sector->number_of_root_directory_entries * sizeof(FileInformation));
		return loaded_address;
	}
	else return NULL; // The file is not found.
}

void primary_ATA_hard_disk_interrupt_handler(void)
{
	finish_interruption(IRQ_PRIMARY_ATA_HARD_DISK);
	print_serial("primary ATA hard disk interrupt\n");
}

void save_file(char const *file_name, unsigned char const *content, unsigned int length)
{
	prohibit_switch_task();
	FileInformation *file_information = get_file_information(file_name);
	unsigned short number_of_necessary_clusters = (length + cluster_size - 1) / cluster_size;
	unsigned short cluster_number;
	unsigned short next_cluster_number;
	Time time = get_current_time();
	char const *dot = strchr(file_name, '.');
	char const *prefix_begin = file_name;
	char const *prefix_end = dot && (unsigned int)dot - (unsigned int)file_name <= _countof(file_information->name) ? dot : file_name + _countof(file_information->name);
	char const *suffix_begin = dot ? dot + 1 : file_name + _countof(file_information->name);
	char const *suffix_end = strlen(suffix_begin) <= _countof(file_information->extension) ? suffix_begin + strlen(suffix_begin) : suffix_begin + _countof(file_information->extension);
	if(file_information)
	{
		if(file_information->flags & FILE_INFORMATION_FLAG_READ_ONLY_FILE)
		{
			ERROR(); // There is a read only file with same name.
			return;
		}
		delete_file(file_name);
	}
	else file_information = get_unused_file_information();
	for(char *name = file_information->name; name != file_information->name + _countof(file_information->name); name++)*name = prefix_begin != prefix_end ? *prefix_begin++ : '\0';
	for(char *extension = file_information->extension; extension != file_information->extension + _countof(file_information->extension); extension++)*extension = suffix_begin != suffix_end ? *suffix_begin++ : '\0';
	file_information->flags = FILE_INFORMATION_FLAG_NORMAL_FILE;
	for(char *reserved = file_information->reserved; reserved != file_information->reserved + _countof(file_information->reserved); reserved++)*reserved = 0x00;
	file_information->time = ((unsigned short)time.hour << 11) + ((unsigned short)time.minute << 5) + (unsigned short)time.second / 2;
	file_information->date = ((time.year - 1980) << 9) + ((unsigned short)time.month << 5) + (unsigned short)time.day;
	file_information->cluster_number = get_unused_cluster_number();
	file_information->size = length;
	sector_flags[address2sector_number(file_information)] |= SECTOR_FLAG_CHANGED;
	cluster_number = file_information->cluster_number;
	for(unsigned short i = 0; i < number_of_necessary_clusters; i++)
	{
		void *cluster_address = get_cluster(cluster_number);
		memcpy(cluster_address, content, cluster_size);
		sector_flags[address2sector_number(cluster_address)] |= SECTOR_FLAG_CHANGED;
		content += cluster_size;
		set_next_cluster_number(cluster_number, no_more_clusters);
		if(i < number_of_necessary_clusters - 1){
			next_cluster_number = get_unused_cluster_number();
			set_next_cluster_number(cluster_number, next_cluster_number);
			cluster_number = next_cluster_number;
		}
	}
	allow_switch_task();
}

void secondary_ATA_hard_disk_interrupt_handler(void)
{
	finish_interruption(IRQ_SECONDARY_ATA_HARD_DISK);
	print_serial("secondary ATA hard disk interrupt\n");
}

void *sector_number2address(unsigned int sector_number)
{
	return (void *)((unsigned int)sector_number * (unsigned int)boot_sector->sector_size + (unsigned int)MEMORY_MAP_LOADED_DISK_BEGIN);
}

SectorSpecifier sector_number2sector_specifier(unsigned int sector_number)
{
	SectorSpecifier sector_specifier;
	sector_specifier.cylinder = sector_number / (boot_sector->number_of_heads * boot_sector->number_of_sectors_per_track);
	sector_number -= sector_specifier.cylinder * boot_sector->number_of_heads * boot_sector->number_of_sectors_per_track;
	sector_specifier.head = sector_number / boot_sector->number_of_sectors_per_track;
	sector_number -= sector_specifier.head * boot_sector->number_of_sectors_per_track;
	sector_specifier.sector = sector_number + 1;
	return sector_specifier;
}

void *sector_specifier2address(SectorSpecifier sector_specifier)
{
	return sector_number2address(sector_specifier2sector_number(sector_specifier));
}

unsigned int sector_specifier2sector_number(SectorSpecifier sector_specifier)
{
	return ((unsigned int)sector_specifier.cylinder * (unsigned int)boot_sector->number_of_heads + (unsigned int)sector_specifier.head) * (unsigned int)boot_sector->number_of_sectors_per_track + (unsigned int)sector_specifier.sector - 1;
}

void set_next_cluster_number(unsigned short cluster_number, unsigned short next_cluster_number)
{
	if(cluster_number % 2)
	{
		((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 2] = (unsigned char)(next_cluster_number >> 4);
		((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 1] &= 0x0f;
		((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 1] += (unsigned char)(next_cluster_number << 4);
		((unsigned char **)file_allocation_tables)[1][(cluster_number - 1) / 2 * 3 + 2] = (unsigned char)(next_cluster_number >> 4);
		((unsigned char **)file_allocation_tables)[1][(cluster_number - 1) / 2 * 3 + 1] &= 0x0f;
		((unsigned char **)file_allocation_tables)[1][(cluster_number - 1) / 2 * 3 + 1] += (unsigned char)(next_cluster_number << 4);
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 2])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][(cluster_number - 1) / 2 * 3 + 1])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[1][(cluster_number - 1) / 2 * 3 + 2])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[1][(cluster_number - 1) / 2 * 3 + 1])] |= SECTOR_FLAG_CHANGED;
	}
	else
	{
		((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3 + 1] &= 0xf0;
		((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3 + 1] += (unsigned char)(next_cluster_number >> 8 & 0x0f);
		((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3] = (unsigned char)next_cluster_number;
		((unsigned char **)file_allocation_tables)[1][cluster_number / 2 * 3 + 1] &= 0xf0;
		((unsigned char **)file_allocation_tables)[1][cluster_number / 2 * 3 + 1] += (unsigned char)(next_cluster_number >> 8 & 0x0f);
		((unsigned char **)file_allocation_tables)[1][cluster_number / 2 * 3] = (unsigned char)next_cluster_number;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3 + 1])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[0][cluster_number / 2 * 3])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[1][cluster_number / 2 * 3 + 1])] |= SECTOR_FLAG_CHANGED;
		sector_flags[address2sector_number(&((unsigned char **)file_allocation_tables)[1][cluster_number / 2 * 3])] |= SECTOR_FLAG_CHANGED;
	}
}

void write_entire_disk(void)
{
	for(unsigned char cylinder = 0; cylinder < boot_sector->number_of_sectors / (boot_sector->number_of_heads * boot_sector->number_of_sectors_per_track); cylinder++)for(unsigned char head = 0; head < boot_sector->number_of_heads; head++)for(unsigned char sector = 1; sector <= boot_sector->number_of_sectors_per_track; sector++)
	{
		SectorSpecifier sector_specifier;
		sector_specifier.cylinder = cylinder;
		sector_specifier.head = head;
		sector_specifier.sector = sector;
		if(sector_flags[sector_specifier2sector_number(sector_specifier)] & SECTOR_FLAG_CHANGED)
		{
			write_cluster(sector_specifier);
			sector_flags[sector_specifier2sector_number(sector_specifier)] &= ~SECTOR_FLAG_CHANGED;
		}
		else continue;
	}
}

void write_cluster(SectorSpecifier sector_specifier)
{
	BIOSInterface arguments;
	unsigned char *source_address = sector_specifier2address(sector_specifier);
	unsigned char *buffer_address = MEMORY_MAP_BIOS_BUFFER;
	memcpy(buffer_address, source_address, boot_sector->sector_size);
	printf_serial("Save cylinder %#04.2x, head %#04.2x, sector %#04.2x, source address %p\n", sector_specifier.cylinder, sector_specifier.head, sector_specifier.sector, source_address);
	for(unsigned char *byte = buffer_address; byte != buffer_address + boot_sector->sector_size; byte++)printf_serial("%02.2x%c", *byte, (unsigned int)(byte+ 1) % 0x10 ? ' ' : '\n');
	arguments.ax = 0x0301;
	arguments.cx = (sector_specifier.cylinder << 8) | sector_specifier.sector;
	arguments.bx = (unsigned short)((unsigned int)buffer_address);
	arguments.dx = sector_specifier.head << 8;
	arguments.es = (unsigned short)((unsigned int)buffer_address >> 4 & 0x0000f000);
	arguments.fs = 0x0000;
	arguments.gs = 0x0000;
	bios_interrupt(0x13, arguments);
}

