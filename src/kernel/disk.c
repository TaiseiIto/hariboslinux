#include "disk.h"
#include "memory.h"
#include "pic.h"
#include "serial.h"

BootSector const * const boot_sector = (BootSector const * const)MEMORY_MAP_LOADED_DISK_BEGIN;

void disk_interrupt_handler(void)
{
	finish_interruption(IRQ_DISK);
	print_serial("disk interrupt\n");
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

