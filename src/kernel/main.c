#include "io.h"
#include "serial.h"

#define WHOLE_SEGMENT 0x08
#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x08
#define VRAM_SEGMENT 0x08

void main(void)
{
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	io_writeb(VRAM_SEGMENT, (void *)0x00000000, 0x0f);
	while(1)io_hlt();
}

