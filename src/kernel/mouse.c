#include "event.h"
#include "io.h"
#include "keyboard.h"
#include "mouse.h"
#include "pic.h"
#include "serial.h"

#define MOUSE_COMMAND_GET_ID			0xf2
#define MOUSE_COMMAND_SET_SAMPLE_RATE		0xf3
#define MOUSE_COMMAND_ENABLE_PACKET_STREAMING	0xf4
#define MOUSE_COMMAND_DISABLE_PACKET_STREAMING	0xf5
#define MOUSE_COMMAND_SET_DEFAULTS		0xf6
#define MOUSE_COMMAND_RESET			0xff

unsigned char mouse_id;

void send_to_mouse(unsigned char data);
void set_mouse_sample_rate(unsigned char rate);

unsigned char get_mouse_id(void)
{
	return mouse_id;
}

void init_mouse(void)
{
	// upgrade mouse ID from 0 to 3
	set_mouse_sample_rate(200);
	set_mouse_sample_rate(100);
	set_mouse_sample_rate(80);
	send_to_mouse(MOUSE_COMMAND_GET_ID);
	printf_serial_polling("mouse ACK = %#04x\n", receive_from_keyboard());
	mouse_id = receive_from_keyboard();
	printf_serial_polling("mouse ID = %#04x\n", mouse_id);

	// upgrade mouse ID from 3 to 4
	set_mouse_sample_rate(200);
	set_mouse_sample_rate(200);
	set_mouse_sample_rate(80);
	send_to_mouse(MOUSE_COMMAND_GET_ID);
	printf_serial_polling("mouse ACK = %#04x\n", receive_from_keyboard());
	mouse_id = receive_from_keyboard();
	printf_serial_polling("mouse ID = %#04x\n", mouse_id);

	// enable packet streaming
	send_to_mouse(MOUSE_COMMAND_ENABLE_PACKET_STREAMING);
	printf_serial_polling("mouse ACK = %#04x\n", receive_from_keyboard());
}

void mouse_interrupt_handler(void)
{
	Event event;
	finish_interruption(IRQ_MOUSE);
	event.type = EVENT_TYPE_MOUSE_INTERRUPT;
	event.event_union.mouse_interrupt_event.signal = inb(PORT_KEYBOARD_DATA);
	enqueue_event(&event);
}

void send_to_mouse(unsigned char data)
{
	send_to_keyboard(KEYBOARD_COMMAND_SEND_TO_MOUSE, data);
}

void set_mouse_sample_rate(unsigned char rate)
{
	send_to_mouse(MOUSE_COMMAND_SET_SAMPLE_RATE);
	printf_serial_polling("mouse ACK = %#04x\n", receive_from_keyboard());
	send_to_mouse(rate);
	printf_serial_polling("mouse ACK = %#04x\n", receive_from_keyboard());
}
