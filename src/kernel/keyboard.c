#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void keyboard_interrupt_handler(void)
{
	finish_interruption(IRQ_KEYBOARD);
	print_serial_polling("keyboard interrupt!!!\n");
}

