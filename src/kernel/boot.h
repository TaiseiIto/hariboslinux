#ifndef _BOOT_H_
#define _BOOT_H_

typedef struct
{
	unsigned short memory_size;	// MiB
	unsigned char keyboard_state;
	unsigned char last_loaded_cylinder;
	unsigned char last_loaded_head;
	unsigned char last_loaded_sector;
} __attribute__((packed)) BootInformation;

#endif

