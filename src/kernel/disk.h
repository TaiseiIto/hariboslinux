#ifndef _DISK_H_
#define _DISK_H_

typedef struct _BootSector
{
	unsigned char jump_instruction[3];
	char product_name[8];
	unsigned short sector_size;
	unsigned char number_of_sectors_per_cluster;
	unsigned short first_sector_number;
	unsigned char number_of_file_allocation_tables;
	unsigned short number_of_root_directory_entries;
	unsigned short number_of_sectors;
	unsigned char media_type;
	unsigned short number_of_sectors_per_file_allocation_table;
	unsigned short number_of_sectors_per_track;
	unsigned short number_of_heads;
	unsigned int number_of_hidden_sectors;
	unsigned int number_of_sectors_double_word;
	unsigned char drive_number;
	unsigned char reserved;
	unsigned char boot_signature;
	unsigned int volume_serial_number;
	char volume_label[11];
	char file_system_name[8];
} __attribute__((packed)) BootSector;

extern BootSector const * const boot_sector;

void disk_interrupt_handler(void);
void primary_ATA_hard_disk_interrupt_handler(void);
void secondary_ATA_hard_disk_interrupt_handler(void);

#endif

