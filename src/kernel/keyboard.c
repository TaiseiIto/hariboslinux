#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void init_keyboard(void)
{
	wait_for_keyboard();
	outb(PORT_KEYBOARD_COMMAND, KEYBOARD_COMMAND_SET_MODE);
	wait_for_keyboard();
	outb(PORT_KEYBOARD_DATA, KEYBOARD_MODE_KEYBOARD_INTERRUPT | KEYBOARD_MODE_MOUSE_INTERRUPT | KEYBOARD_MODE_SYSTEM_FLAG | KEYBOARD_MODE_SCANCODE01);
}

void keyboard_interrupt_handler(void)
{
	unsigned char signal;
	finish_interruption(IRQ_KEYBOARD);
	signal = inb(PORT_KEYBOARD_DATA);
	printf_serial_polling("keyboard signal = %#04X\n", signal);
}

void wait_for_keyboard(void)
{
	while(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_UNSENDABLE);
}

