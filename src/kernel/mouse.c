#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "serial.h"

#define MOUSE_COMMAND_GET_ID			0xf2
#define MOUSE_COMMAND_ENABLE_PACKET_STREAMING	0xf4
#define MOUSE_COMMAND_DISABLE_PACKET_STREAMING	0xf5
#define MOUSE_COMMAND_SET_DEFAULTS		0xf6
#define MOUSE_COMMAND_RESET			0xff

void init_mouse(void)
{
	send_to_mouse(MOUSE_COMMAND_SET_DEFAULTS);
	send_to_mouse(MOUSE_COMMAND_RESET);
	outb(PORT_KEYBOARD_COMMAND, KEYBOARD_COMMAND_GET_COMPAQ_STATUS);
	outb(PORT_KEYBOARD_COMMAND, KEYBOARD_COMMAND_ENABLE_AUXILIARY_DEVICE);
	send_to_mouse(MOUSE_COMMAND_GET_ID);
	send_to_mouse(MOUSE_COMMAND_ENABLE_PACKET_STREAMING);
}

void mouse_interrupt_handler(void)
{
	unsigned char signal;
	finish_interruption(IRQ_MOUSE);
	signal = inb(PORT_KEYBOARD_DATA);
	printf_serial_polling("mouse signal = %#04X\n", signal);
}

void send_to_mouse(unsigned char data)
{
	send_to_keyboard(KEYBOARD_COMMAND_SEND_TO_MOUSE, data);
}

