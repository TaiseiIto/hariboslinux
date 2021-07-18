#include "io.h"
#include "serial.h"

#define WHOLE_SEGMENT 0x08
#define KERNEL_CODE_SEGMENT 0x10
#define KERNEL_DATA_SEGMENT 0x18
#define STACK_SEGMENT 0x20
#define VRAM_SEGMENT 0x28

typedef struct
{
	unsigned short memory_size;	// MiB
	unsigned short screen_width;
	unsigned short screen_height;
	unsigned char bits_per_pixel;
	unsigned char keyboard_state;
} BootInformation;

void main(void)
{
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	writeb(VRAM_SEGMENT, (void *)0x00000000, 0x0f);
	while(1)hlt();
}

