#include "keyboard.h"
#include "serial.h"

void keyboard_interrupt_handler(void)
{
	print_serial_polling("keyboard interrupt!!!\n");
}

