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

typedef struct _FileIntormation
{
	char name[8];
	char extension[3];
	unsigned char flags;
	#define FILE_INFORMATION_FLAG_READ_ONLY_FILE	0x01
	#define FILE_INFORMATION_FLAG_HIDDEN_FILE	0x02
	#define FILE_INFORMATION_FLAG_SYSTEM_FILE	0x04
	#define FILE_INFORMATION_FLAG_DISK_NAME		0x08
	#define FILE_INFORMATION_FLAG_DIRECTORY		0x10
	#define FILE_INFORMATION_FLAG_NORMAL_FILE	0x20
	char reserved[10];
	unsigned short time;
	#define FILE_INFORMATION_TIME_MASK_BISECOND	0x001f
	#define FILE_INFORMATION_TIME_MASK_MINUTE	0x07e0
	#define FILE_INFORMATION_TIME_MASK_HOUR		0xf800
	unsigned short date;
	#define FILE_INFORMATION_DATE_MASK_DAY		0x001f
	#define FILE_INFORMATION_DATE_MASK_MONTH	0x01e0
	#define FILE_INFORMATION_DATE_MASK_YEAR		0xfe00
	unsigned short cluster_number;
	unsigned int size;
} FileInformation;

extern BootSector const * const boot_sector;

char *create_file_name(FileInformation const *file_information);
void disk_interrupt_handler(void);
void init_file_system(void);
void primary_ATA_hard_disk_interrupt_handler(void);
void secondary_ATA_hard_disk_interrupt_handler(void);

#endif

