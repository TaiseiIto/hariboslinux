#include "boot.h"
#include "font.h"
#include "gdt.h"
#include "graphic.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "serial.h"
#include "stdio.h"

void main(void)
{
	new_line_serial_polling();
	print_serial_polling("Hello, kernel.bin!\n\n");
	init_gdt();
	init_idt();
	init_pic();
	sti();
	init_keyboard();
	init_mouse();
	new_line_serial_polling();
	while(1)hlt();
}

