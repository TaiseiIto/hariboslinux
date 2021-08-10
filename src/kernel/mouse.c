#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "serial.h"

void init_mouse(void)
{
	wait_to_send_to_keyboard();
	outb(PORT_KEYBOARD_COMMAND, KEYBOARD_COMMAND_SEND_TO_MOUSE);
	wait_to_send_to_keyboard();
	outb(PORT_KEYBOARD_DATA, MOUSE_ENABLE_PACKET_STREAMING);
}

void mouse_interrupt_handler(void)
{
	unsigned char signal;
	finish_interruption(IRQ_MOUSE);
	signal = inb(PORT_KEYBOARD_DATA);
	printf_serial_polling("mouse signal = %#04X\n", signal);
}

