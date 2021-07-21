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

// get BootInformation structure at 0x00007bf8
BootInformation get_boot_information(void);
void __stack_chk_fail(void);

void main(void)
{
	BootInformation boot_information;
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n");
	new_line_serial_polling();
	boot_information = get_boot_information();
	print_serial_polling("memory size = 0x");
	print_word_hex_serial_polling(boot_information.memory_size);
	new_line_serial_polling();
	print_serial_polling("screen width = 0x");
	print_word_hex_serial_polling(boot_information.screen_width);
	new_line_serial_polling();
	print_serial_polling("screen height = 0x");
	print_word_hex_serial_polling(boot_information.screen_height);
	new_line_serial_polling();
	print_serial_polling("bits per pixel = 0x");
	print_byte_hex_serial_polling(boot_information.bits_per_pixel);
	new_line_serial_polling();
	print_serial_polling("keyboard state = 0x");
	print_byte_hex_serial_polling(boot_information.keyboard_state);
	new_line_serial_polling();
	writeb(VRAM_SEGMENT, (void *)0x00000000, 0x0f);
	while(1)hlt();
}

// get BootInformation structure at 0x00007bf8
BootInformation get_boot_information(void)
{
	BootInformation boot_information;
	reads(WHOLE_SEGMENT, (void *)0x00007bf8, (void *)&boot_information, sizeof(boot_information));
	return boot_information;
}

void __stack_chk_fail(void)
{
	print_serial_polling("__stack_chk_fail called!\n");
}

