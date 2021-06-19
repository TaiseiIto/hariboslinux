// Usage: pack OUTPUT BOOTSECTOR [FILE] ...
//
// This program outputs Haribos Linux floppy disk raw image file named by the argument OUTPUT.
// The output includes the files specified by the arguments [FILE] ...

#include <stdio.h>
#include <stdlib.h>

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
	char const *output_file_name;
	// Haribos Linux floppy disk boot sector binary file name
	char const *boot_sector_file_name;
	// file names included in the floppy disk
	char const * const *input_file_names;
	// num of [FILE] ...
	unsigned int num_of_input_files;
	// "pack", "OUTPUT" and "BOOOTSECTOR"
	unsigned int const num_of_necessary_args = 3;

	// related to boot sector
	// read from boot_sector_file
	BootSector *boot_sector;
	void *boot_sector_image;
	unsigned int const boot_sector_size = 0x00000200;
	// Haribos Linux floppy disk boot sector binary file
	FILE *boot_sector_file;

	// check argc
	if(argc < num_of_necessary_args)
	{
		fprintf(stderr, "Usage: pack OUTPUT BOOTSECTOR [FILE] ...\n");
		return EXIT_FAILURE;
	}
	// print argv
	for(int argc_i = 0; argc_i < argc; argc_i++)printf("argv[%d] : %s\n", argc_i, argv[argc_i]);
	// decode argv
	output_file_name = argv[1];
	boot_sector_file_name = argv[2];
	input_file_names = argv + num_of_necessary_args;
	num_of_input_files = argc - num_of_necessary_args;
	printf("output file : %s\n", output_file_name);
	printf("boot sector bynari file : %s\n", boot_sector_file_name);
	for(unsigned int num_of_input_files_i = 0; num_of_input_files_i < num_of_input_files; num_of_input_files_i++)printf("input file [%u] : %s\n", num_of_input_files_i, input_file_names[num_of_input_files_i]);
	// decode boot sector
	if((boot_sector_file = fopen(boot_sector_file_name, "rb")) == NULL)
	{
		fprintf(stderr, "Can't open %s\n", boot_sector_file_name);
		return EXIT_FAILURE;
	}
	if((boot_sector_image = malloc(boot_sector_size)) == NULL)
	{
		fprintf(stderr, "Can't alloc boot sector image!\n");
		return EXIT_FAILURE;
	}
	if(fread(boot_sector_image, 1, boot_sector_size, boot_sector_file) < boot_sector_size)
	{
		fprintf(stderr, "Can't read %s\n", boot_sector_file_name);
		return EXIT_FAILURE;
	}
	boot_sector = boot_sector_image;
	printf("jump instructions[0] : %#04X\n", boot_sector->jump_instructions[0]);
	free(boot_sector_image);
	if((fclose(boot_sector_file)) == EOF)
	{
		fprintf(stderr, "Can't close %s\n", boot_sector_file_name);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

