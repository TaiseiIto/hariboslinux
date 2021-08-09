#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void keyboard_interrupt_handler(void)
{
	finish_interruption(IRQ_KEYBOARD);
	inb(0x0060);
	print_serial_polling("keyboard interrupt!!!\n");
}

