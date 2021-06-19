// Usage: pack OUTPUT BOOTSECTOR [FILE] ...
//
// This program outputs Haribos Linux floppy disk raw image file named by the argument OUTPUT.
// The output includes the files specified by the arguments [FILE] ...

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _countof(array) (sizeof(array) / sizeof(array[0]))

typedef struct
{
	unsigned char jump_instructions[3];

	// The string doesn't end with '\0'.
	// Margin is filled with spaces.
	char product_name[8];

	unsigned short num_of_bytes_per_sector;
	unsigned char num_of_sectors_per_cluster;

	// Number of reserved sectors.
	// The boot record sectors are included in this value.
	// After the sectors, FAT sectors begin.
	unsigned short num_of_reserved_sectors;

	unsigned char num_of_FATs;
	unsigned short num_of_root_directory_entries;
	unsigned short num_of_sectors_in_disk;
	unsigned char media_type;
	unsigned short num_of_sectors_per_FAT;
	unsigned short num_of_sectors_per_track;
	unsigned short num_of_heads;
	unsigned int num_of_hidden_sectors;
	unsigned int large_num_of_sectors_in_disk;
	unsigned char drive_number;
	unsigned char reserved;

	// If it's 0x29, it's bootable;
	unsigned char boot_signature;

	unsigned int volume_serial_number;

	// The strings doesn't end with '\0'.
	// Margin is filled with spaces.
	char volume_label[11];
	char file_system_name[8];
} __attribute__((packed)) BootSector;

int main(int argc, char const * const * const argv)
{
	// related to command line arguments
	// Haribos Linux floppy disk raw image file name
	char const *floppy_disk_raw_image_file_name;
	// Haribos Linux floppy disk boot sector binary file name
	char const *boot_sectors_file_name;
	// file names included in the floppy disk
	char const * const *input_file_names;
	// num of [FILE] ...
	unsigned int num_of_input_files;
	// "pack", "OUTPUT" and "BOOOTSECTOR"
	unsigned int const num_of_necessary_args = 3;

	// related to boot sector
	// read from boot_sector_file
	BootSector *boot_sector_structure;
	// Haribos Linux floppy disk boot sector binary file
	FILE *boot_sectors_file;
	void *boot_sectors;

	// related to output
	FILE *floppy_disk_raw_image_file;
	void *floppy_disk_raw_image;
	unsigned int floppy_disk_size;

	// check argc
	if(argc < num_of_necessary_args)
	{
		fprintf(stderr, "Usage: pack OUTPUT BOOTSECTOR [FILE] ...\n");
		return EXIT_FAILURE;
	}
	// print argv
	for(int argc_i = 0; argc_i < argc; argc_i++)printf("argv[%d] : %s\n", argc_i, argv[argc_i]);
	// decode argv
	floppy_disk_raw_image_file_name = argv[1];
	boot_sectors_file_name = argv[2];
	input_file_names = argv + num_of_necessary_args;
	num_of_input_files = argc - num_of_necessary_args;
	printf("floppy disk raw image file : %s\n", floppy_disk_raw_image_file_name);
	printf("boot sector bynari file : %s\n", boot_sectors_file_name);
	for(unsigned int num_of_input_files_i = 0; num_of_input_files_i < num_of_input_files; num_of_input_files_i++)printf("input file [%u] : %s\n", num_of_input_files_i, input_file_names[num_of_input_files_i]);
	// decode boot sector
	if((boot_sectors_file = fopen(boot_sectors_file_name, "rb")) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", boot_sectors_file_name);
		return EXIT_FAILURE;
	}
	if((boot_sector_structure = malloc(sizeof(*boot_sector_structure))) == NULL)
	{
		fprintf(stderr, "Can't alloc boot sector structure!\n");
		return EXIT_FAILURE;
	}
	if(fread(boot_sector_structure, sizeof(*boot_sector_structure), 1, boot_sectors_file) < 1)
	{
		fprintf(stderr, "Can't read %s\n", boot_sectors_file_name);
		return EXIT_FAILURE;
	}
	for(unsigned int i = 0; i < _countof(boot_sector_structure->jump_instructions); i++)printf("jump instructions[%d] : %#04X\n", i, boot_sector_structure->jump_instructions[i]);
	printf("produce name : ");
	for(unsigned int i = 0; i < _countof(boot_sector_structure->product_name); i++)printf("%c", boot_sector_structure->product_name[i]);
	printf("\n");
	printf("%#06X bytes per sector\n", boot_sector_structure->num_of_bytes_per_sector);
	printf("%#04X sectors per cluster\n", boot_sector_structure->num_of_sectors_per_cluster);
	printf("%#06X reserved sectors\n", boot_sector_structure->num_of_reserved_sectors);
	printf("%#04X FATs\n", boot_sector_structure->num_of_FATs);
	printf("%#06X root directory entries\n", boot_sector_structure->num_of_root_directory_entries);
	printf("%#06X sectors in disk\n", boot_sector_structure->num_of_sectors_in_disk);
	printf("media type : %#04X\n", boot_sector_structure->media_type);
	printf("%#06X sectors per FAT\n", boot_sector_structure->num_of_sectors_per_FAT);
	printf("%#06X sectors per track\n", boot_sector_structure->num_of_sectors_per_track);
	printf("%#06X heads\n", boot_sector_structure->num_of_heads);
	printf("%#010X hidden sectors\n", boot_sector_structure->num_of_hidden_sectors);
	printf("%#010X sectors in disk\n", boot_sector_structure->large_num_of_sectors_in_disk);
	printf("drive number : %#04X\n", boot_sector_structure->drive_number);
	printf("boot signature : %#04X\n", boot_sector_structure->boot_signature);
	printf("volume serial number : %#010X\n", boot_sector_structure->volume_serial_number);
	printf("volume label : ");
	for(unsigned int i = 0; i < _countof(boot_sector_structure->volume_label); i++)printf("%c", boot_sector_structure->volume_label[i]);
	printf("\n");
	printf("file system name : ");
	for(unsigned int i = 0; i < _countof(boot_sector_structure->file_system_name); i++)printf("%c", boot_sector_structure->file_system_name[i]);
	printf("\n");
	// read boot sectors
	boot_sectors = malloc(boot_sector_structure->num_of_reserved_sectors * boot_sector_structure->num_of_bytes_per_sector);
	if(fseek(boot_sectors_file, 0, SEEK_SET))
	{
		fprintf(stderr, "Can't seek the start point of %s\n", boot_sectors_file_name);
		return EXIT_FAILURE;
	}
	if(fread(boot_sectors, 1, boot_sector_structure->num_of_reserved_sectors * boot_sector_structure->num_of_bytes_per_sector, boot_sectors_file) < boot_sector_structure->num_of_reserved_sectors * boot_sector_structure->num_of_bytes_per_sector)
	{
		fprintf(stderr, "Can't read %s\n", boot_sectors_file_name);
		return EXIT_FAILURE;
	}
	if(fclose(boot_sectors_file) == EOF)
	{
		fprintf(stderr, "Can't close %s\n", boot_sectors_file_name);
		return EXIT_FAILURE;
	}
	// init floppy disk raw image
	floppy_disk_size = boot_sector_structure->large_num_of_sectors_in_disk * boot_sector_structure->num_of_bytes_per_sector;
	floppy_disk_raw_image = malloc(floppy_disk_size);
	memset(floppy_disk_raw_image, 0, floppy_disk_size);
	// locate the boot sectors
	memcpy(floppy_disk_raw_image, boot_sectors, boot_sector_structure->num_of_reserved_sectors * boot_sector_structure->num_of_bytes_per_sector);
	// write floppy disk raw image
	if((floppy_disk_raw_image_file = fopen(floppy_disk_raw_image_file_name, "wb")) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", floppy_disk_raw_image_file_name);
		return EXIT_FAILURE;
	}
	if(fwrite(floppy_disk_raw_image, 1, floppy_disk_size, floppy_disk_raw_image_file) < floppy_disk_size)
	{
		fprintf(stderr, "Can't write %s\n", floppy_disk_raw_image_file_name);
		return EXIT_FAILURE;
	}
	if(fclose(floppy_disk_raw_image_file) == EOF)
	{
		fprintf(stderr, "Can't close %s\n", floppy_disk_raw_image_file_name);
		return EXIT_FAILURE;
	}
	free(floppy_disk_raw_image);
	free(boot_sector_structure);
	return EXIT_SUCCESS;
}

