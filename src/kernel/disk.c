#include "disk.h"
#include "pic.h"
#include "serial.h"

void disk_interrupt_handler(void)
{
	finish_interruption(IRQ_DISK);
	print_serial_polling("disk interrupt\n");
}

void primary_ATA_hard_disk_interrupt_handler(void)
{
	finish_interruption(IRQ_PRIMARY_ATA_HARD_DISK);
	print_serial_polling("primary ATA hard disk interrupt\n");
}

void secondary_ATA_hard_disk_interrupt_handler(void)
{
	finish_interruption(IRQ_SECONDARY_ATA_HARD_DISK);
	print_serial_polling("secondary ATA hard disk interrupt\n");
}

