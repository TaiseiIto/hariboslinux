#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void keyboard_interrupt_handler(void)
{
	unsigned char keycode;
	finish_interruption(IRQ_KEYBOARD);
	keycode = inb(0x0060);
	printf_serial_polling("keycode = %#04X\n", keycode);
}

