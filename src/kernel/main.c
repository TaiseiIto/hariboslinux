#include "io.h"
#include "serial.h"

#define WHOLE_SEGMENT 0x08
#define KERNEL_CODE_SEGMENT 0x10
#define KERNEL_DATA_SEGMENT 0x18
#define STACK_SEGMENT 0x20
#define VRAM_SEGMENT 0x28

void main(void)
{
	BootInformation boot_information;
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	boot_information = get_boot_information();
	writeb(VRAM_SEGMENT, (void *)0x00000000, 0x0f);
	while(1)hlt();
}

