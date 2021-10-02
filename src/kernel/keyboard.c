#include "event.h"
#include "io.h"
#include "keyboard.h"
#include "pic.h"
#include "serial.h"

void decode_keyboard_interrupt(unsigned char signal)
{
}

void init_keyboard(void)
{
	send_to_keyboard(KEYBOARD_COMMAND_SET_MODE, KEYBOARD_MODE_KEYBOARD_INTERRUPT | KEYBOARD_MODE_MOUSE_INTERRUPT | KEYBOARD_MODE_SYSTEM_FLAG | KEYBOARD_MODE_SCANCODE01);
}

void keyboard_interrupt_handler(void)
{
	Event event;
	finish_interruption(IRQ_KEYBOARD);
	event.type = EVENT_TYPE_KEYBOARD_INTERRUPT;
	event.event_union.keyboard_interrupt_event.signal = inb(PORT_KEYBOARD_DATA);
	enqueue_event(&event);
}

unsigned char receive_from_keyboard(void)
{
	wait_to_receive_from_keyboard();
	return inb(PORT_KEYBOARD_DATA);
}

void send_to_keyboard(unsigned short command, unsigned char data)
{
	wait_to_send_to_keyboard();
	outb(PORT_KEYBOARD_COMMAND, command);
	wait_to_send_to_keyboard();
	outb(PORT_KEYBOARD_DATA, data);
}

void wait_to_send_to_keyboard(void)
{
	while(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_UNSENDABLE);
}

void wait_to_receive_from_keyboard(void)
{
	while(!(inb(PORT_KEYBOARD_STATUS) & KEYBOARD_STATUS_RECEIVED));
}

